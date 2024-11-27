#ifndef _ARGUMENT_HPP_
#define _ARGUMENT_HPP_

#include <concepts>
#include <type_traits>
#include <vector>
#include <memory>
#include <string_view>
#include <iostream>

#include <lib/arg_parser/store/store.hpp>

namespace argument_parser {

class Argument {
 public:
  enum FoundClasses { NOT_FOUND, WAS_FOUND, WAS_INITIALIZE };
 public:
  Argument() = default;
  Argument(const Argument& value) = delete;
  Argument(Argument&& value);
  Argument& operator=(Argument&& value);
  Argument(std::string_view full_name,
    std::string_view short_name, std::string_view descriprion);
  Argument(std::string_view full_name, std::string_view descriprion);
  Argument(std::string_view full_name);
  ~Argument() = default;

 public:
  bool convert(std::string_view string_data);

  template<typename ValueType>
  auto GetData();

  template<typename ValueType>
  auto GetMultiData();

 public:
  template<typename StoreType>
  Argument& SetStore(Store<StoreType>* store_ptr);

 public:
  inline bool IsMultivalue() const { return is_multivalue_; };
  inline bool IsPositional() const { return is_positional_; };
  inline FoundClasses GetStatus() { return is_found_; };


  inline void WasFound() { is_found_ = FoundClasses::WAS_FOUND; };
  inline void WasInitialize() { is_found_ = FoundClasses::WAS_INITIALIZE; };
  inline Argument& Positional() {
    is_positional_ = true;
    return *this;
  };

  template<typename StoreType>
  Argument& SetMultiValueStore(MultiValueStore<StoreType>* store_ptr);

  inline std::string_view GetFullName() const { return full_name_; };
  inline std::string_view GetShortName() const { return short_name_; };

  inline const BaseStore* GetStorePtr() const { return store_.get(); };

 private:
  std::string_view full_name_ = "";
  std::string_view short_name_ = "";
  std::string_view descriprion_ = "";
  bool is_multivalue_ = false;
  bool is_positional_ = false;
  FoundClasses is_found_ = FoundClasses::NOT_FOUND;

  std::unique_ptr<BaseStore> store_ = nullptr;
#ifdef LABA4
 public:
  std::size_t min_val = 0;

  inline std::size_t GetStoreCount() { return store_->GetCountOfData(); };

  template<typename Type>
  void SetPtrStore(Type* ptr);

  template<typename Type>
  void SetPtrMultiValueStore(Type* ptr);
#endif
};

template<typename ValueType>
auto Argument::GetData() {
  if (store_.get()) {
    return dynamic_cast<Store<ValueType>*>(store_.get())->data_;
  }
  return ValueType{};
};


template<typename ValueType>
auto Argument::GetMultiData() {
  try {
    if ( typeid(*store_) == typeid(MultiValueStore<ValueType>)) {
      return dynamic_cast<MultiValueStore<ValueType>*>(store_.get())->data_;
    }
  } catch (const std::bad_typeid& ex) {
      std::cout << ex.what() << '\n';
  }
  return ValueType{};
};

template<typename StoreType>
Argument& Argument::SetStore(Store<StoreType>* store_ptr) {
  if (store_) {
    store_.reset();
  }
  store_ = std::move(std::unique_ptr<Store<StoreType>>(store_ptr));
  return *this;
};

template<>
Argument& Argument::SetStore<bool>(Store<bool>* store_ptr);

template<typename StoreType>
Argument& Argument::SetMultiValueStore(MultiValueStore<StoreType>* store_ptr) {
  is_multivalue_ = true;
  if (store_) {
    store_.reset();
  }
  store_ = std::move(std::unique_ptr<MultiValueStore<StoreType>>(store_ptr));

  return *this;
};

#ifdef LABA4
template<typename Type>
void Argument::SetPtrStore(Type* ptr) {
  if (store_.get()) {
    dynamic_cast<Store<Type>&>(*store_).ptr_ = ptr;
  } else {
    store_ = std::unique_ptr<Store<Type>>();
    dynamic_cast<Store<Type>&>(*store_).ptr_ = ptr;
  }
};

template<typename Type>
void Argument::SetPtrMultiValueStore(Type* ptr) {
  if (store_.get()) {
    dynamic_cast<MultiValueStore<Type>&>(*store_).ptr_ = ptr;
  } else {
    store_ = std::unique_ptr<MultiValueStore<Type>>();
    dynamic_cast<MultiValueStore<Type>&>(*store_).ptr_ = ptr;
  }
};
#endif

template<typename StoreType, typename... ConstructArgumentTypes>
Argument make_argument(ConstructArgumentTypes&&... construct_args) {
  Argument arg((construct_args, ...));
  return arg;
};

} // argment_parser

#endif // _ARGUMENT_HPP_