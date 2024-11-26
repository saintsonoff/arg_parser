#include "argument.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <array>
#include <vector>

#include <arg_parser.hpp>
#include <argument.hpp>
#include <store.hpp>
// #include <arg_parser/arg_parser.h>

// #define DEFAULT_MAIN
#ifndef DEFAULT_MAIN

int main(int argc, char** argv) {

#define CORRECT_INPUT
#ifndef CORRECT_INPUT
  std::vector<std::string_view> argv_test = {
    "name_of_prog",
    "--flag=493",
    "-f=fsdfsdf",
    "-flag", "4",
    "-bbbb=true",
    "-bool_flag",
    "--abcdefg", "111111111",
    "12234234234234"
  };
#else
  std::vector<std::string_view> argv_test = {
    "name_of_prog",
    "--integer=493",
    "--flag",
    "--str=privet_mir",
    "--dbl", "30.239",
  };
#endif
  argument_parser::ArgParser parser_device;

  argument_parser::Argument arg;
  arg.SetStore<int>(nullptr);
  arg.SetStore(new argument_parser::Store<std::string>());

  auto argument_value = argument_parser::make_argument<int>("--integer");

  parser_device.registrate(
    argument_parser::make_argument<int>("integer").SetStore(new argument_parser::Store<int>()),
    argument_parser::make_argument<bool>("flag").SetStore(new argument_parser::Store<bool>()),
    argument_parser::make_argument<std::string>("str").SetStore(new argument_parser::Store<std::string>()),
    argument_parser::make_argument<double>("dbl").SetStore(new argument_parser::Store<double>())
  );
  parser_device.parse(argv_test);
  return 0;
}
#else 

#include <functional>
#include <arg_parser/arg_parser.h>
// #include <lib/arg_parser/arg_parser.h>

#include <iostream>
#include <numeric>

struct Options {
  bool sum = false;
  bool mult = false;
};

int main(int argc, char** argv) {
  Options opt;
  std::vector<int> values;

  ArgumentParser::ArgParser parser("Program");
  parser.AddIntArgument("N").MultiValue(1).Positional().StoreValues(values);
  parser.AddFlag("sum", "add args").StoreValue(opt.sum);
  parser.AddFlag("mult", "multiply args").StoreValue(opt.mult);
  parser.AddHelp('h', "help", "Program accumulate arguments");

  if(!parser.Parse(argc, argv)) {
    std::cout << "Wrong argument" << std::endl;
    std::cout << parser.HelpDescription() << std::endl;
    return 1;
  }

  if(parser.Help()) {
    std::cout << parser.HelpDescription() << std::endl;
    return 0;
  }

  if(opt.sum) {
    std::cout << "Result: " << std::accumulate(values.begin(), values.end(), 0) << std::endl;
  } else if(opt.mult) {
    std::cout << "Result: " << std::accumulate(values.begin(), values.end(), 1, std::multiplies<int>()) << std::endl;
  } else {
    std::cout << "No one options had chosen" << std::endl;
    std::cout << parser.HelpDescription();
    return 1;
  }

  return 0;
}

#endif // DEFAULT_MAIN