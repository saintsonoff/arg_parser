#ifndef _LEXER_HPP_
#define _LEXER_HPP_

#include <vector>
#include <string_view>
#include <memory>

#include <argument/argument.hpp>

namespace argument_parser {

namespace lexeme {

  struct Lexeme {
    Lexeme(std::string_view value);
    virtual Lexeme* GetOwner();
    virtual void SetOwner(Lexeme* new_owner_ptr);
    virtual ~Lexeme() = 0;
    std::string_view value_;
  };

  struct Value : public Lexeme {
    Value(std::string_view value);
    ~Value() override = default;
    Lexeme* GetOwner() override;
    void SetOwner(Lexeme* new_owner_ptr) override;
    Lexeme* owner_ptr = nullptr;
  };

  struct FullName : public Lexeme {
    FullName(std::string_view value);
    ~FullName() override = default;
  };

  struct ShortName : public Lexeme {
    ShortName(std::string_view value);
    ~ShortName() override = default;
  };

} // lexeme

class Lexer {
 public:
  using LexemContType = std::vector<std::shared_ptr<lexeme::Lexeme>>;
  static std::vector<std::string_view>
    StrongSplit(const std::vector<std::string_view>& argv);

  static LexemContType Lexing(const std::vector<std::string_view>& argv);

  static LexemContType SemanticLexing(LexemContType& lexemes,
    std::vector<Argument>& uments);
};

} // argument_pareser

#endif // _LEXER_HPP_