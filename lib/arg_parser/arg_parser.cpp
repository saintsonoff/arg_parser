#include "parser/parser.hpp"
#include <arg_parser.hpp>

#include <stdexcept>
#include <iostream>

#include <parser.hpp>
#include <lexer.hpp>

namespace argument_parser {

bool ArgParser::parse(std::vector<std::string_view>& argv) {
  LexerDevice lexer;
  try {
    lexer.Run(argv, args_);
  } catch (std::runtime_error& ex){
    std::cerr << ex.what() << std::endl;
    return false;
  }

  auto[lexemes_cont, positional_lexemes_cont] = lexer.GetData();

  ParserDevice parser;
  try {
    parser.Run(args_, lexemes_cont, positional_lexemes_cont);
  } catch (std::runtime_error& ex){
    std::cerr << ex.what() << std::endl;
    return false;
  }

  return true;
};

void ArgParser::ClearArguments() {
  args_.clear();
};

std::string ArgParser::GetDescriptions() {
  std::string full_description;
  full_description.reserve((
      std::strlen("   <--") +
      std::strlen(">, <-") +
      std::strlen(">  ::  [") +
      std::strlen("]  ||  ") +
      10
    ) * args_.size()
  );

  for (auto&& arg : args_) {
    full_description += "   <--";
    full_description += {arg.GetFullName().begin(), arg.GetFullName().end()};
    full_description += ">, ";
    if (arg.GetShortName() != "") {
      full_description += "<-";
      full_description += {arg.GetShortName().begin(), arg.GetShortName().end()};
      full_description += ">  ::  ";
    }
    full_description += "[";
    full_description += arg.GetStrStoreType();
    full_description += "]  ||  ";
    full_description += {arg.GetDescription().begin(), arg.GetDescription().end()};
    full_description += "\n";
  }

  return full_description;
};

} // argument_parser