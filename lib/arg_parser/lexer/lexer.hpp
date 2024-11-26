#ifndef _LEXER_HPP_
#define _LEXER_HPP_

#include <memory>
#include <stdexcept>
#include <vector>
#include <string_view>
#include <algorithm>
#include <type_traits>
#include <iterator>
#include <utility>

#include <argument/argument.hpp>

namespace argument_parser {

namespace lexeme {

  struct Lexeme {
    Lexeme(std::string_view value);
    virtual Lexeme* GetOwner();
    virtual void SetOwner(Lexeme* new_owner_ptr);
    virtual ~Lexeme() = 0;
    std::string_view value_;
  };

  struct Value : public Lexeme {
    Value(std::string_view value);
    ~Value() override = default;
    Lexeme* GetOwner() override;
    void SetOwner(Lexeme* new_owner_ptr) override;
    Lexeme* owner_ptr = nullptr;
  };

  struct FullName : public Lexeme {
    FullName(std::string_view value);
    ~FullName() override = default;
  };

  struct ShortName : public Lexeme {
    ShortName(std::string_view value);
    ~ShortName() override = default;
  };

} // lexeme

namespace {

template<typename InItr>
concept is_arg_cont_itr = requires(InItr itr) {
  std::input_iterator<InItr>;
  {itr->IsMultivalue()} -> std::same_as<bool>;
  {itr->IsPositional()} -> std::same_as<bool>;
  {itr->GetFullName()} -> std::same_as<std::string_view>;
  {itr->GetShortName()} -> std::same_as<std::string_view>;
};

} // namespace

class LexerDevice {
 public:
  using LexemContType = std::vector<std::shared_ptr<lexeme::Lexeme>>;
 public:
  void Run(const std::vector<std::string_view>& argv, std::vector<Argument>& arguments);
  inline LexemContType GetLexemes() { return lexemes_cont_; };
  inline LexemContType GetPositionalCandidats() { return position_lexemes_cont_; };
  inline std::pair<LexemContType, LexemContType> GetData() {
     return {position_lexemes_cont_, lexemes_cont_};
  };

 private:
  template<std::input_iterator InItr>
  std::vector<std::string_view>
    StrongSplit(InItr argv_begin, InItr argv_end);

  template<std::input_iterator InItr>
  void Lexing(InItr argv_begin, InItr argv_end);

  template<is_arg_cont_itr InItr>
  void SemanticLexing(InItr arguments_begin, InItr arguments_end);
 private:
  LexemContType position_lexemes_cont_;
  LexemContType lexemes_cont_;
};


template<std::input_iterator InItr>
std::vector<std::string_view>
  LexerDevice::StrongSplit(InItr argv_begin, InItr argv_end) {
  constexpr std::string_view separator_charapter = "=";

  std::vector<std::string_view> strong_split_argv;

  for(;argv_begin != argv_end; ++argv_begin) {
    auto& arg = *argv_begin;
    if (auto separator_pos = arg.find(separator_charapter);
      separator_pos != arg.npos) {
      strong_split_argv.emplace_back(
        arg.begin(),
        arg.begin() + separator_pos);
      strong_split_argv.emplace_back(
        arg.begin() + separator_pos + 1,
        arg.end());
    } else {
      strong_split_argv.emplace_back(std::move(arg));
    }
  }

  return strong_split_argv;
};

namespace {

template<typename LexemeType>
void PushBackLexeme(LexerDevice::LexemContType& cont,
 std::string_view::iterator begin_itr, std::string_view::iterator end_itr) {
  cont.emplace_back(
    std::make_shared<LexemeType>(
      std::string_view(begin_itr, end_itr)));
};

} // namespace

template<std::input_iterator InItr>
void LexerDevice::Lexing(InItr argv_begin, InItr argv_end) {
  constexpr std::string_view short_argument_prefix = "-";
  constexpr std::string_view full_argument_prefix = "--";

  for (;argv_begin != argv_end; ++argv_begin) {
    auto& arg = *argv_begin;
    if (arg.contains(full_argument_prefix)) {
      PushBackLexeme<lexeme::FullName>(lexemes_cont_,
        arg.begin() + full_argument_prefix.size(),
        arg.end());
    } else if (arg.contains(short_argument_prefix)) {
      PushBackLexeme<lexeme::ShortName>(lexemes_cont_,
        arg.begin() + short_argument_prefix.size(),
        arg.end());
    } else {
      PushBackLexeme<lexeme::Value>(lexemes_cont_,
        arg.begin(),
        arg.end());
    }
  }
};

namespace {

enum SearchArgStatus {
  MULTIVALUE, UNITVALUE, NOT_FOUND, FLAG,
  POSITIONAL_UNITVALUE, POSITIONAL_MULTIVALUE,
};

template<typename NameType, is_arg_cont_itr InItr>
SearchArgStatus IsContainLexeme(InItr begin_itr, InItr end_itr, lexeme::Lexeme& arg_lexeme) {
  if constexpr (std::is_same_v<NameType, lexeme::ShortName>) { // instance by short argname
    if (auto res_itr = std::find_if(begin_itr, end_itr,
      [&arg_lexeme](std::iterator_traits<decltype(begin_itr)>::reference arg) {
        if (arg_lexeme.value_ == arg.GetShortName())
          return true;
        return false;
    }); res_itr != end_itr) {
      if (typeid(*(res_itr->GetStorePtr())) == typeid(Store<bool>&)) {
        return SearchArgStatus::FLAG;
      }
      if (res_itr->IsMultivalue() && res_itr->IsPositional()) {
        return SearchArgStatus::POSITIONAL_MULTIVALUE;
      } else if (!res_itr->IsMultivalue() && res_itr->IsPositional()) {
        return SearchArgStatus::POSITIONAL_UNITVALUE;
      } else if (res_itr->IsMultivalue() && !res_itr->IsPositional()) {
        return SearchArgStatus::MULTIVALUE;
      } else {
        return SearchArgStatus::UNITVALUE;
      }
    } else {
      return SearchArgStatus::NOT_FOUND;
    }
  } else if constexpr (std::is_same_v<NameType, lexeme::FullName>) { // instance by full argname
    if (auto res_itr = std::find_if(begin_itr, end_itr,
      [&arg_lexeme](std::iterator_traits<decltype(begin_itr)>::reference arg) {
        if (arg_lexeme.value_ == arg.GetFullName())
          return true;
        return false;
    }); res_itr != end_itr) {
      if (typeid(*(res_itr->GetStorePtr())) == typeid(Store<bool>&)) {
        return SearchArgStatus::FLAG;
      }
      if (res_itr->IsMultivalue() && res_itr->IsPositional()) {
        return SearchArgStatus::POSITIONAL_MULTIVALUE;
      } else if (!res_itr->IsMultivalue() && res_itr->IsPositional()) {
        return SearchArgStatus::POSITIONAL_UNITVALUE;
      } else if (res_itr->IsMultivalue() && !res_itr->IsPositional()) {
        return SearchArgStatus::MULTIVALUE;
      } else {
        return SearchArgStatus::UNITVALUE;
      }
    } else {
      return SearchArgStatus::NOT_FOUND;
    }
  } else { // another types is baned
    static_assert(true, "lexeme is not flagname type");
  }
};

template<std::input_iterator InItr>
SearchArgStatus CheckStatus(InItr begin_itr, InItr end_itr, lexeme::Lexeme& arg_lexeme) {
  if (typeid(arg_lexeme) == typeid(lexeme::ShortName)) {
    if (auto search_res = IsContainLexeme<lexeme::ShortName>(begin_itr, end_itr, arg_lexeme);
      search_res != SearchArgStatus::NOT_FOUND) {
      return search_res;
    } else {
      std::string error_message = "Argument found, but not retistrate:   \"";
      error_message += arg_lexeme.value_;
      error_message += "\"\n";
      throw std::runtime_error(error_message);
    }
  } else if (typeid(arg_lexeme) == typeid(lexeme::FullName)) {
    if (auto search_res = IsContainLexeme<lexeme::FullName>(begin_itr, end_itr, arg_lexeme);
      search_res != SearchArgStatus::NOT_FOUND) {
      return search_res;
    } else {
      std::string error_message = "Argument found, but not retistrate:\n   \"";
      error_message += arg_lexeme.value_;
      error_message += "\"\n";
      throw std::runtime_error(error_message);
    }
  }
  return SearchArgStatus::NOT_FOUND;
};

template<std::input_iterator InItr>
auto SetOwnerToRange(InItr begin_itr, InItr end_itr, lexeme::Lexeme* owner) {
  for (;begin_itr != end_itr; ++begin_itr) {
    if (typeid(*begin_itr) != typeid(lexeme::Value))
      break;
  
    (*begin_itr)->SetOwner(owner);
  }
  return begin_itr;
};

template<std::input_iterator InItr>
auto SetOwner(InItr begin_itr, InItr end_itr, lexeme::Lexeme* owner) {
  if (begin_itr != end_itr) {
    (*begin_itr)->SetOwner(owner);
  }
  return begin_itr;
};

} // namespace

template<is_arg_cont_itr InItr>
void LexerDevice::SemanticLexing(InItr arguments_begin, InItr arguments_end) {
  for (auto begin_itr = std::begin(lexemes_cont_), end_itr = std::end(lexemes_cont_);
    begin_itr != end_itr; ++begin_itr) {
    auto status = CheckStatus(arguments_begin, arguments_end, **begin_itr);
    if (status == SearchArgStatus::NOT_FOUND) {
    } else if (status == SearchArgStatus::MULTIVALUE) {
      begin_itr = SetOwnerToRange(begin_itr + 1, end_itr, begin_itr->get());
    } else if (status == SearchArgStatus::UNITVALUE) {
      begin_itr = SetOwner(begin_itr + 1, end_itr, begin_itr->get());
    }
  }

  LexemContType position_candidats_cont;
  LexemContType clear_lexemes_cont;
  for (auto&& elem : lexemes_cont_) {
    if (typeid(*elem) == typeid(lexeme::Value) &&
      !elem->GetOwner()) {
      position_candidats_cont.push_back(elem);
    } else {
      clear_lexemes_cont.push_back(elem);
    };
  }

  lexemes_cont_ = clear_lexemes_cont;
  position_lexemes_cont_ = position_candidats_cont;

};

} // argument_pareser

#endif // _LEXER_HPP_