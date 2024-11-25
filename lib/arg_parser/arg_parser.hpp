#ifndef _ARG_PARSER_HPP_
#define _ARG_PARSER_HPP_



#include <vector>
#include <string_view>

#include <argument.hpp>

namespace argument_parser {

class ArgParser {

 public:
  void registrate();
  bool parse(std::vector<std::string_view> argv);
 private:
  std::vector<Argument> args_;
};

} // argument_parser

#endif // _ARG_PARSER_HPP_