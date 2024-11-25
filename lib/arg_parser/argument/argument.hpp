#ifndef _ARGUMENT_HPP_
#define _ARGUMENT_HPP_

#include <memory>
#include <string_view>

#include <store.hpp>

namespace argument_parser {

class Argument {
 public:
  inline bool IsMultivalue() const { return is_multivalue_; };
  inline bool IsPositional() const { return is_positional_; };
  inline std::string_view GetFullName() const { return full_name_; };
  inline std::string_view GetShortName() const { return short_name_; };
  inline BaseStore& GetStore() const { return *store_; };
 private:
  std::string_view full_name_;
  std::string_view short_name_;
  bool is_multivalue_ = false;
  bool is_positional_ = false;
  std::shared_ptr<BaseStore> store_;
};

} // argument_parser

#endif // _ARGUMENT_HPP_