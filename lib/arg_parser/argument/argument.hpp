#ifndef _ARGUMENT_HPP_
#define _ARGUMENT_HPP_

#include <memory>
#include <string_view>
#include <iostream>

#include <store.hpp>

namespace argument_parser {

class Argument {
 public:
  Argument() = default;
  Argument(const Argument& value) = delete;
  Argument(Argument&& value);
  Argument& operator=(Argument&& value);
  Argument(std::string_view full_name,
    std::string_view short_name, std::string_view descriprion);
  Argument(std::string_view full_name, std::string_view descriprion);
  Argument(std::string_view full_name);

 public:
  template<typename StoreType>
  Argument& SetStore(Store<StoreType>* store_ptr);

 public:
  inline bool IsMultivalue() const { return is_multivalue_; };
  inline bool IsPositional() const { return is_positional_; };

  inline std::string_view GetFullName() const { return full_name_; };
  inline std::string_view GetShortName() const { return short_name_; };

  inline const BaseStore* GetStorePtr() const { return store_.get(); };

 private:
  std::string_view full_name_ = "";
  std::string_view short_name_ = "";
  std::string_view descriprion_ = "";
  bool is_multivalue_ = false;
  bool is_positional_ = false;
  std::unique_ptr<BaseStore> store_ = nullptr;
};

template<typename StoreType>
Argument& Argument::SetStore(Store<StoreType>* store_ptr) {
  if (store_) {
    store_.reset();
  }
  store_ = std::move(std::unique_ptr<Store<StoreType>>(store_ptr));
  return *this;
};

template<typename StoreType, typename... ConstructArgumentTypes>
Argument make_argument(ConstructArgumentTypes&&... construct_args) {
  Argument arg((construct_args, ...));
  return arg;
};

} // argment_parser

#endif // _ARGUMENT_HPP_