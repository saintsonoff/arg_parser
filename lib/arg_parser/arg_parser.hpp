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
  void ClearArguments();

  template<typename ValueType>
  auto GetValue(std::string_view arg_name);

  template<typename ValueType>
  auto GetMultiValue(std::string_view arg_name);

 private:
  template<typename ArgumentType>
  void registrate_single(ArgumentType&& arg);

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
  if constexpr (sizeof...(args)) {
    args_.reserve(args_.size() + sizeof...(args));
    (registrate_single(std::forward<ArgumentType>(args)), ...);
  }
};

template<typename ArgumentType>
void ArgParser::registrate_single(ArgumentType&& arg) {
  for (auto&& arg_by_cont : args_) {
    if (arg_by_cont.GetFullName() == arg.GetFullName() &&
      arg_by_cont.GetShortName() == arg.GetShortName()) {
      return;
    }
  }
  
  args_.push_back(std::move(arg));
};

} // argument_parser

#endif // _ARG_PARSER_HPP_