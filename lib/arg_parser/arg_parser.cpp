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

} // argument_parser