#include <arg_parser.hpp>

#include <stdexcept>
#include <iostream>

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
  for (auto&& elem : positional_lexemes_cont) {
    std::cout << typeid(*elem).name() << std::endl;
  }
  return true;
};

} // argument_parser