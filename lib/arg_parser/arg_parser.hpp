#ifndef _ARG_PARSER_HPP_
#define _ARG_PARSER_HPP_

#include <type_traits>
#include <utility>
#include <vector>
#include <string_view>

#include <argument.hpp>

namespace argument_parser {

class ArgParser {
 public:
  template<typename... ArgumentType>
  void registrate(ArgumentType&&... args);

  bool parse(std::vector<std::string_view>& argv);

 private:
  std::vector<Argument> args_;
};

template<typename... ArgumentType>
void ArgParser::registrate(ArgumentType&&... args) {
  static_assert((std::is_same_v<std::remove_reference_t<ArgumentType>, Argument> && ...),
    "\nArgParser::registrate\n   all arguments must have Argument type\n");

  args_.reserve(args_.size() + sizeof...(args));
  (args_.push_back(std::move(args)), ...);
};

} // argument_parser

#endif // _ARG_PARSER_HPP_