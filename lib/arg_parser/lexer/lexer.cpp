#include "lexer.hpp"
#include <lexer.hpp>

#include <iterator>
#include <memory>
#include <string_view>
#include <vector>

namespace argument_parser {

namespace lexeme {

Lexeme::Lexeme(std::string_view value) : value_(value) {  };

std::shared_ptr<lexeme::Lexeme> Lexeme::GetOwner() { return nullptr; };

void Lexeme::SetOwner(std::shared_ptr<lexeme::Lexeme> new_owner_ptr) {  };

Lexeme::~Lexeme() {  };

Value::Value(std::string_view value) : Lexeme(value) {  };

std::shared_ptr<lexeme::Lexeme> Value::GetOwner() { return owner_ptr; };

void Value::SetOwner(std::shared_ptr<lexeme::Lexeme> new_owner_ptr) { owner_ptr = std::shared_ptr<Lexeme>(new_owner_ptr); };

FullName::FullName(std::string_view value) : Lexeme(value) {  };

ShortName::ShortName(std::string_view value) : Lexeme(value) {  };

} // lexeme

void LexerDevice::Run(const std::vector<std::string_view>& argv, std::vector<Argument>& arguments) {
  auto strong_split_argv = StrongSplit(std::begin(argv), std::end(argv));

  Lexing(std::begin(strong_split_argv), std::end(strong_split_argv));
  SemanticLexing(std::begin(arguments), std::end(arguments));
};


} // argument_parser