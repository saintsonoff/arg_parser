#include "ArgParser.hpp"

#include <memory> 
#include <string>
#include <vector>
#include <string_view>
#include <algorithm>

#include <argument.hpp>

namespace ArgumentParser {
ArgumentLabwork::ArgumentLabwork(argument_parser::Argument arg) : arg_(std::move(arg)) {  };

ArgParserLabwork::ArgParserLabwork(std::string_view parser_name) : parser_name_(parser_name) {  };

ArgumentLabwork& ArgParserLabwork::AddIntArgument(std::string_view full_name) {
  return AddIntArgument(full_name, "");
};

ArgumentLabwork& ArgParserLabwork::AddIntArgument(std::string_view full_name, std::string_view descriprion) {
  return AddIntArgument('\0', full_name, descriprion);
};

ArgumentLabwork& ArgParserLabwork::AddIntArgument(char short_name, std::string_view full_name) {
  return AddIntArgument(short_name, full_name, "");
};

ArgumentLabwork& ArgParserLabwork::AddIntArgument(char short_name, std::string_view full_name,
  std::string_view descriprion) {
  short_name_cont_.emplace_back(new char[2]{short_name, '\0'});
  argument_parser::Argument arg{full_name, short_name_cont_.back().get(), descriprion};

  arg.SetStore(new argument_parser::Store<int>{});

  argument_labwork_cont_.emplace_back(std::move(arg));

  return argument_labwork_cont_.back();
};

ArgumentLabwork& ArgParserLabwork::AddStringArgument(std::string_view full_name) {
  return AddStringArgument(full_name, "");
};

ArgumentLabwork& ArgParserLabwork::AddStringArgument(std::string_view full_name, std::string_view descriprion) {
  return AddStringArgument('\0', full_name, descriprion);
};

ArgumentLabwork& ArgParserLabwork::AddStringArgument(char short_name, std::string_view full_name) { 
  return AddStringArgument(short_name, full_name, "");
};

ArgumentLabwork& ArgParserLabwork::AddStringArgument(char short_name, std::string_view full_name, std::string_view descriprion) {

  short_name_cont_.emplace_back(new char[2]{short_name, '\0'});
  argument_parser::Argument arg{full_name, short_name_cont_.back().get(), descriprion};  arg.SetStore(new argument_parser::Store<std::string>{});

  argument_labwork_cont_.emplace_back(std::move(arg));

  return argument_labwork_cont_.back();
};

ArgumentLabwork& ArgParserLabwork::AddFlag(std::string_view full_name) {
  return AddFlag(full_name, "");
};

ArgumentLabwork& ArgParserLabwork::AddFlag(std::string_view full_name, std::string_view descriprion) {
  return AddFlag('\0', full_name, descriprion);
};

ArgumentLabwork& ArgParserLabwork::AddFlag(char short_name, std::string_view full_name) {
  return AddFlag(short_name, full_name, "");
}

ArgumentLabwork& ArgParserLabwork::AddFlag(char short_name, std::string_view full_name, std::string_view descriprion) {
  short_name_cont_.emplace_back(new char[2]{short_name, '\0'});
  argument_parser::Argument arg{full_name, short_name_cont_.back().get(), descriprion};  arg.SetStore(new argument_parser::Store<bool>{});
  argument_labwork_cont_.emplace_back(std::move(arg));

  return argument_labwork_cont_.back();
};

void ArgParserLabwork::AddHelp(std::string_view full_name) {
  AddHelp(full_name, "");
};

void ArgParserLabwork::AddHelp(std::string_view full_name, std::string_view descriprion) {
  AddHelp('\0', full_name, descriprion);
};

void ArgParserLabwork::AddHelp(char short_name, std::string_view full_name) {
  AddHelp(short_name, full_name, "");
}

void ArgParserLabwork::AddHelp(char short_name, std::string_view full_name, std::string_view descriprion) {
  short_name_cont_.emplace_back(new char[2]{short_name, '\0'});
  argument_parser::Argument arg{full_name, short_name_cont_.back().get(), descriprion};  arg.SetStore(new argument_parser::Store<bool>{});
  arg.WasFound();

  argument_labwork_cont_.emplace_back(std::move(arg));

  help_name_ = full_name;
};

bool ArgParserLabwork::Help() {
  if (help_name_.empty())
    return false;
  return arg_parser_device_.GetValue<bool>(help_name_);
};

std::string ArgParserLabwork::HelpDescription() {
  std::string full_description{parser_name_.begin(), parser_name_.end()};
  full_description += "\n";
  full_description += arg_parser_device_.GetDescriptions();
  return full_description;
};

bool ArgParserLabwork::Parse(int argc, char** argv) {
  std::vector<std::string> argv_cont;

  std::for_each(argv, argv + argc, [&argv_cont](char* elem){
    argv_cont.emplace_back(elem);
  });

  return Parse(argv_cont);
};

bool ArgParserLabwork::Parse(const std::vector<std::string>& argv) {
  for (auto&& arg : argument_labwork_cont_) {
    arg_parser_device_.registrate(arg.GetArg());
  }

  std::vector<std::string_view> argv_sv_cont;
  std::for_each(std::begin(argv) + 1, std::end(argv), [&argv_sv_cont](const std::string& elem){
    argv_sv_cont.emplace_back(elem);
  });

  auto parse_res = arg_parser_device_.parse(argv_sv_cont);

  if (Help()) {
    std::cout << HelpDescription() << std::endl;
    return true;
  }
  return parse_res;
};

bool ArgParserLabwork::GetFlag(std::string_view name) {
  return arg_parser_device_.GetValue<bool>(name);
};

std::string ArgParserLabwork::GetStringValue(std::string_view name) {
  return arg_parser_device_.GetValue<std::string>(name);
};

std::vector<std::string> ArgParserLabwork::GetStringValues(std::string_view name) {
  return arg_parser_device_.GetMultiValue<std::vector<std::string>>(name);
};

std::string ArgParserLabwork::GetStringValue(std::string_view name, std::size_t ind) {
  return (arg_parser_device_.GetMultiValue<std::vector<std::string>>(name))[ind];
};

int ArgParserLabwork::GetIntValue(std::string_view name) {
  return arg_parser_device_.GetValue<int>(name);
};

int ArgParserLabwork::GetIntValue(std::string_view name, std::size_t ind) {
  return (arg_parser_device_.GetValue<int>(name));
};

std::vector<int> ArgParserLabwork::GetIntValues(std::string_view name) {
  return arg_parser_device_.GetMultiValue<std::vector<int>>(name);
};
ArgParserLabwork::~ArgParserLabwork() {
  short_name_cont_.clear();
};

} // ArgumentParser