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
    virtual std::shared_ptr<Lexeme> GetOwner();
    virtual void SetOwner(std::shared_ptr<Lexeme> new_owner_ptr);
    virtual ~Lexeme() = 0;
    std::string_view value_;
  };

  struct Value : public Lexeme {
    Value(std::string_view value);
    ~Value() override = default;
    std::shared_ptr<Lexeme> GetOwner() override;
    void SetOwner(std::shared_ptr<Lexeme> new_owner_ptr) override;
    std::shared_ptr<Lexeme> owner_ptr = nullptr;
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

bool IsNumber(std::string_view short_argument) {
  if (short_argument.size() > 1 && (short_argument[1] >= '0' && short_argument[1] <= '9'))
    return true;
  return false;
};

} // namespace


template<std::input_iterator InItr>
void LexerDevice::Lexing(InItr argv_begin, InItr argv_end) {
  constexpr std::string_view short_argument_prefix = "-";
  constexpr std::string_view full_argument_prefix = "--";

  for (;argv_begin != argv_end; ++argv_begin) {
    auto& arg = *argv_begin;
    if (arg.starts_with(full_argument_prefix)) {
      PushBackLexeme<lexeme::FullName>(lexemes_cont_,
        arg.begin() + full_argument_prefix.size(),
        arg.end());
    } else if (arg.starts_with(short_argument_prefix) &&
      !IsNumber(arg) && arg.size() == 2) {
      PushBackLexeme<lexeme::ShortName>(lexemes_cont_,
        arg.begin() + short_argument_prefix.size(),
        arg.end());
    } else if (arg.starts_with(short_argument_prefix) &&
      !IsNumber(arg) && arg.size() > 2) {
        for (auto beg_itr = std::begin(arg) + 1, end_itr = std::end(arg);
          beg_itr != end_itr; ++beg_itr) {
          PushBackLexeme<lexeme::ShortName>(lexemes_cont_,
          beg_itr,
          beg_itr + 1);
          }
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
        if (arg_lexeme.value_ == arg.GetShortName()) {
          arg.WasFound();
          return true;
        } else {
          return false;
        }
    }); res_itr != end_itr) {
      try {
        if (typeid(*(res_itr->GetStorePtr())) == typeid(Store<bool>&)) {
          std::for_each(begin_itr, end_itr,
            [&arg_lexeme](std::iterator_traits<decltype(begin_itr)>::reference arg) {
            if (arg_lexeme.value_ == arg.GetShortName())
              arg.convert("1");
          });

          return SearchArgStatus::FLAG;
        }
      } catch (const std::bad_typeid& ex) {
        std::cout << ex.what() << '\n';
        // return SearchArgStatus::NOT_FOUND;
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
        if (arg_lexeme.value_ == arg.GetFullName()) {
          arg.WasFound();
          return true;
        } else {
          return false;
        }
    }); res_itr != end_itr) {
      try {
        if (typeid(*(res_itr->GetStorePtr())) == typeid(Store<bool>&)) {
          std::for_each(begin_itr, end_itr,
            [&arg_lexeme](std::iterator_traits<decltype(begin_itr)>::reference arg) {
            if (arg_lexeme.value_ == arg.GetFullName())
              arg.convert("1");
          });
          return SearchArgStatus::FLAG;
        }
      } catch (const std::bad_typeid& ex) {
        std::cout << ex.what() << '\n';
        // return SearchArgStatus::NOT_FOUND;
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
  try {
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
  } catch (const std::bad_typeid& ex) {
    std::cout << ex.what() << '\n';
  }
  return SearchArgStatus::NOT_FOUND;
};

template<std::random_access_iterator RAItr>
auto SetOwnerToRange(RAItr begin_itr, RAItr end_itr, std::shared_ptr<lexeme::Lexeme> owner) {
  for (;begin_itr != end_itr; ++begin_itr) {
    try {
      if (typeid(**begin_itr) != typeid(lexeme::Value))
        break;
    } catch (const std::bad_typeid& ex) {
      std::cout << ex.what() << '\n';
      return end_itr;
    }
    (*begin_itr)->SetOwner(owner);
  }
  return --begin_itr;
};

template<std::input_iterator InItr>
auto SetOwnerByItr(InItr begin_itr, InItr end_itr, std::shared_ptr<lexeme::Lexeme> owner) {
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
    if (status == SearchArgStatus::FLAG) {
      // begin_itr->WasFound();
    } else if (status == SearchArgStatus::MULTIVALUE) {
      begin_itr = SetOwnerToRange(begin_itr + 1, end_itr, *begin_itr);
    } else if (status == SearchArgStatus::UNITVALUE) {
      begin_itr = SetOwnerByItr(begin_itr + 1, end_itr, *begin_itr);
    }
  }

  LexemContType position_candidats_cont;
  LexemContType clear_lexemes_cont;
  for (auto&& elem : lexemes_cont_) {
    try {
      if (typeid(*elem) == typeid(lexeme::Value)) {
        if(elem->GetOwner()) {
          clear_lexemes_cont.push_back(elem);
        } else {
          position_candidats_cont.push_back(elem);
        }
      }
    } catch (const std::bad_typeid& ex) {
      std::cout << ex.what() << '\n';
    }
  }

  lexemes_cont_ = clear_lexemes_cont;
  position_lexemes_cont_ = position_candidats_cont;

};

} // argument_pareser

#endif // _LEXER_HPP_