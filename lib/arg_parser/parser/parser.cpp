#include <parser.hpp>

#include <stdexcept>

#include <lexer/lexer.hpp>
#include <argument/argument.hpp>

namespace argument_parser {

void ParserDevice::Run(std::vector<Argument>& args,
  const LexerDevice::LexemContType& positional_lexemes_cont,
  const LexerDevice::LexemContType& lexemes_cont) {
  decltype(auto) pos_lex_beg = std::begin(positional_lexemes_cont);
  decltype(auto) pos_lex_end = std::end(positional_lexemes_cont);

  for (auto&& arg : args) {
    if (arg.IsPositional()) {
      bool is_parse = true;
      while (is_parse && pos_lex_beg != pos_lex_end) {
        is_parse = arg.convert((*pos_lex_beg)->value_);
        ++pos_lex_beg;
      }
    } else {
      for (auto&& lexeme : lexemes_cont) {
#if 0
        std::cout << "=====================" << std::endl;
        std::cout << "owner " << lexeme->GetOwner()->value_ << std::endl;
        std::cout << "full " << arg.GetFullName() << std::endl;
        std::cout << "short " << arg.GetShortName() << std::endl;
        std::cout << "=====================" << std::endl;
#endif
        if (lexeme->GetOwner()->value_ == arg.GetFullName() ||
            lexeme->GetOwner()->value_ == arg.GetShortName()) {
          bool is_parse = arg.convert(lexeme->value_);
          if (!is_parse && arg.GetStatus() == Argument::NOT_FOUND) {
            std::string error_message = "parse fail, cannot convert arg\n   from value: ";
            error_message += lexeme->value_;
            error_message += "\n   to argument: ";
            error_message += arg.GetFullName();
            throw std::runtime_error(error_message);
          }
        }
      }
    }
  }

  for (auto&& arg : args) {
    if (arg.GetStatus() == Argument::FoundClasses::NOT_FOUND) {
      std::string error_message = "parse fail, cannot find arg\n   full name: ";
      error_message += arg.GetFullName();
      error_message += "\n   short name: ";
      error_message += arg.GetShortName();
      throw std::runtime_error(error_message);
    } else if (arg.GetStatus() == Argument::FoundClasses::WAS_FOUND) {
      std::cerr << "Arg was not initialized:\n" 
        << "   full name: " << arg.GetFullName() << "\n"
        << "   short name: " << arg.GetShortName() << std::endl;
    }
  }

#if LABA4
  for (auto&& arg : args) {
    if (arg.IsMultivalue() && arg.min_val > arg.GetStoreCount()) {
      std::string error_message = "parse fail, minimal count of multivalue not found arg\n   full name: ";
      error_message += arg.GetFullName();
      error_message += "\n   short name: ";
      error_message += arg.GetShortName();
      throw std::runtime_error(error_message);
    }
  }
#endif // LABA4
};

}