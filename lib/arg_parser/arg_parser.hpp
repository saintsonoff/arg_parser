#ifndef _ARG_PARSER_HPP_
#define _ARG_PARSER_HPP_

#include <type_traits>
#include <utility>
#include <vector>
#include <string_view>

#include <lib/arg_parser/argument/argument.hpp>

namespace argument_parser {

class ArgParser {
 public:
  template<typename... ArgumentType>
  void registrate(ArgumentType&&... args);

  bool parse(std::vector<std::string_view>& argv);

  template<typename ValueType>
  auto GetValue(std::string_view arg_name);

  template<typename ValueType>
  auto GetMultiValue(std::string_view arg_name);

 private:
  std::vector<Argument> args_;
};

template<typename ValueType>
auto ArgParser::GetValue(std::string_view arg_name) {
  for (auto&& elem : args_) {
    if (elem.GetFullName() == arg_name ||
        elem.GetShortName() == arg_name)
      return elem.GetData<ValueType>();
  }
  return ValueType{};
};

template<typename ValueType>
auto ArgParser::GetMultiValue(std::string_view arg_name) {
  for (auto&& elem : args_) {
    if (elem.GetFullName() == arg_name ||
        elem.GetShortName() == arg_name)
      return elem.GetMultiData<ValueType>();
  }
  return ValueType{};
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