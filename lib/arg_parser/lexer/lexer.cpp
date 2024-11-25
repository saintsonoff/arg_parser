#include <lexer.hpp>

#include <memory>
#include <stdexcept>
#include <vector>
#include <string_view>
#include <algorithm>
#include <type_traits>
#include <iterator>

namespace argument_parser {

namespace lexeme {

Lexeme::Lexeme(std::string_view value) : value_(value) {  };

Lexeme* Lexeme::GetOwner() { return nullptr; };

Lexeme::~Lexeme() {  };

Value::Value(std::string_view value) : Lexeme(value) {  };

Lexeme* Value::GetOwner() { return owner_ptr; };

void Value::SetOwner(Lexeme* new_owner_ptr) { owner_ptr = new_owner_ptr; };

FullName::FullName(std::string_view value) : Lexeme(value) {  };

ShortName::ShortName(std::string_view value) : Lexeme(value) {  };

} // lexeme

std::vector<std::string_view>
  Lexer::StrongSplit(const std::vector<std::string_view>& argv) {
  constexpr std::string_view separator_charapter = "=";

  std::vector<std::string_view> strong_split_argv;

  for(auto&& arg : argv) {
    if (auto separator_pos = arg.find(separator_charapter);
      separator_pos == arg.npos) {
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
void PushBackLexeme(Lexer::LexemContType& cont,
 std::string_view::iterator begin_itr, std::string_view::iterator end_itr) {
  cont.emplace_back(
    std::make_shared<LexemeType>(
      std::string_view(begin_itr, end_itr)));
};

} // namespace

Lexer::LexemContType Lexing(const std::vector<std::string_view>& argv) {
  constexpr std::string_view short_argument_prefix = "-";
  constexpr std::string_view full_argument_prefix = "--";

  std::vector<std::shared_ptr<lexeme::Lexeme>> strong_split_argv;

  for (auto&& arg : argv) {
    if (arg.contains(full_argument_prefix)) {
      PushBackLexeme<lexeme::FullName>(strong_split_argv,
        arg.begin() + full_argument_prefix.size(),
        arg.end());
    } else if (arg.contains(short_argument_prefix)) {
      PushBackLexeme<lexeme::ShortName>(strong_split_argv,
        arg.begin() + short_argument_prefix.size(),
        arg.end());
    } else {
      PushBackLexeme<lexeme::Value>(strong_split_argv,
        arg.begin(),
        arg.end());
    }
  }

  return strong_split_argv;
};

namespace {

enum SearchArgStatus {
  MULTIVALUE, UNITVALUE, NOT_FOUND, FLAG,
  POSITIONAL_UNITVALUE, POSITIONAL_MULTIVALUE,
};

template<typename InItr>
concept is_arg_cont_itr = requires(InItr itr) {
  std::input_iterator<InItr>;
  {itr->IsMultivalue()} -> std::same_as<bool>;
  {itr->IsPositional()} -> std::same_as<bool>;
  {itr->GetFullName()} -> std::same_as<std::string_view>;
  {itr->GetShortName()} -> std::same_as<std::string_view>;
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
      if (typeid(res_itr->GetStore()) == typeid(Store<bool>&)) {
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
      if (typeid(res_itr->GetStore()) == typeid(Store<bool>&)) {
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
  ++begin_itr;
  if (begin_itr != end_itr) {
    (*begin_itr)->SetOwner(owner);
  }
  ++begin_itr;
  return begin_itr;
};
} // namespace

Lexer::LexemContType Lexer::SemanticLexing(LexemContType& lexemes,
  std::vector<Argument>& arguments) {
  for (auto begin_itr = std::begin(lexemes), end_itr = std::end(lexemes);
    begin_itr != end_itr; ++begin_itr) {
    auto status = CheckStatus(std::begin(arguments), std::end(arguments), **begin_itr);
    if (status == SearchArgStatus::UNITVALUE) {
      begin_itr = SetOwner(begin_itr + 1, end_itr, begin_itr->get());
    } else if (status == SearchArgStatus::MULTIVALUE) {
      begin_itr = SetOwnerToRange(begin_itr + 1, end_itr, begin_itr->get());
    }
  }
  return lexemes;
};

} // argument_parser