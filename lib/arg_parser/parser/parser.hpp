#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include <lexer/lexer.hpp>
#include <argument/argument.hpp>

namespace argument_parser {

class ParserDevice {
 public:
  static void Run(std::vector<Argument>& args,
    const LexerDevice::LexemContType& positional_lexemes_cont,
    const LexerDevice::LexemContType& lexemes_cont);
};

}

#endif // _PARSER_HPP_