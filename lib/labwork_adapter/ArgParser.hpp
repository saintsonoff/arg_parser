#ifndef _ARG_PARSER_LABWORK4_HPP_
#define _ARG_PARSER_LABWORK4_HPP_

#include <string_view>
#include <memory>
#include <type_traits>

#include <lib/arg_parser/arg_parser.hpp>
#include <lib/arg_parser/argument/argument.hpp>
#include <lib/arg_parser/store/store.hpp>

namespace ArgumentParser {

class ArgumentLabwork {
 public:
  ArgumentLabwork(argument_parser::Argument arg);
  ArgumentLabwork(ArgumentLabwork&& value) : arg_(std::move(value.arg_)) {  };
  ArgumentLabwork& operator=(ArgumentLabwork&& value) {
    arg_ = std::move(value.arg_);
    return *this;
  };
  // ArgumentLabwork(argument_parser::Argument arg);

  template<typename Type>
  ArgumentLabwork& StoreValue(Type&& store_ptr);

  template<typename Type>
  ArgumentLabwork& StoreValues(Type&& store_ptr);

  template<typename Type>
  ArgumentLabwork& MultiValue(int count = 0);
  inline ArgumentLabwork& Positional() { arg_.Positional(); return *this; };

  template<typename Type>
  ArgumentLabwork& Default(Type&& default_value);

  inline argument_parser::Argument GetArg() { return std::move(arg_); };
 private:
  argument_parser::Argument arg_;
};

template<typename Type>
ArgumentLabwork& ArgumentLabwork::Default(Type&& default_value) {
  arg_.SetStore(new argument_parser::Store<Type>{std::move(default_value)});
  // arg_.WasFound();
  arg_.WasInitialize();
  return *this;
};

template<typename Type>
ArgumentLabwork& ArgumentLabwork::StoreValue(Type&& store_ptr) {
  arg_.SetStore(new argument_parser::Store<std::remove_reference_t<Type>>());
  arg_.SetPtrStore(&store_ptr);
  return *this;
}

template<typename Type>
ArgumentLabwork& ArgumentLabwork::StoreValues(Type&& store_ptr) {
  arg_.SetMultiValueStore(new argument_parser::MultiValueStore<std::remove_reference_t<Type>>());
  arg_.SetPtrMultiValueStore(&store_ptr);
  return *this;
}

template<typename Type>
ArgumentLabwork& ArgumentLabwork::MultiValue(int count) {
  arg_.SetMultiValueStore(new argument_parser::MultiValueStore<std::vector<Type>>()).min_val = count;
  return *this;
};

class ArgParserLabwork {
 public:
  ArgParserLabwork(std::string_view parser_name);
  ~ArgParserLabwork();

 public:
  ArgumentLabwork& AddIntArgument(std::string_view full_name);
  ArgumentLabwork& AddIntArgument(std::string_view full_name, std::string_view descriprion);
  ArgumentLabwork& AddIntArgument(char short_name, std::string_view full_name);
  ArgumentLabwork& AddIntArgument(char short_name, std::string_view full_name, std::string_view descriprion);
  int GetIntValue(std::string_view name);
  int GetIntValue(std::string_view name, std::size_t ind);
  std::vector<int> GetIntValues(std::string_view name);

  ArgumentLabwork& AddStringArgument(std::string_view full_name);
  ArgumentLabwork& AddStringArgument(std::string_view full_name, std::string_view descriprion);
  ArgumentLabwork& AddStringArgument(char short_name, std::string_view full_name);
  ArgumentLabwork& AddStringArgument(char short_name, std::string_view full_name, std::string_view descriprion);
  std::string GetStringValue(std::string_view name);
  std::string GetStringValue(std::string_view name, std::size_t ind);
  std::vector<std::string> GetStringValues(std::string_view name);

  ArgumentLabwork& AddFlag(std::string_view full_name);
  ArgumentLabwork& AddFlag(std::string_view full_name, std::string_view descriprion);
  ArgumentLabwork& AddFlag(char short_name, std::string_view full_name);
  ArgumentLabwork& AddFlag(char short_name, std::string_view full_name, std::string_view descriprion);
  bool GetFlag(std::string_view name);

 public:
  void AddHelp(std::string_view full_name);
  void AddHelp(std::string_view full_name, std::string_view descriprion);
  void AddHelp(char short_name, std::string_view full_name);
  void AddHelp(char short_name, std::string_view full_name, std::string_view descriprion);
  bool Help();
  std::string HelpDescription();

 public:
  bool Parse(int argc, char** argv);
  bool Parse(const std::vector<std::string>& argv);
 private:
  std::string_view help_name_;

  argument_parser::ArgParser arg_parser_device_;

  std::vector<ArgumentLabwork> argument_labwork_cont_;
  std::vector<char*> short_name_cont_;

  std::string_view parser_name_;
};

} // ArgumentParser

#endif // _ARG_PARSER_LABWORK4_HPP_