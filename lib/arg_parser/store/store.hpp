#ifndef _STORE_HPP_
#define _STORE_HPP_

#include <string>
#include <string_view>
#include <sstream>

namespace argument_parser {

namespace {

template<typename ContType>
concept IsContainer = 
  requires(ContType cont, ContType::value_type val) {
    typename ContType::value_type;
    typename ContType::size_type;
    typename ContType::iterator;
    typename ContType::const_iterator;
    {cont.push_back(val)};
    {cont.emplace_back(val)};
};

} // namespace

class BaseStore {
 public:
  virtual ~BaseStore() = 0;
#if LABA4
  virtual std::size_t GetCountOfData() const { return 0; };
#endif
  virtual bool string_to_data(const std::string& str_data) = 0;
};

template<typename StorageType>
class Store : public BaseStore {
 public:
  Store() = default;
  Store(const StorageType& data) : data_(data) {  };
  Store(StorageType&& data) : data_(std::move(data)) {  };
  Store(const Store& value) = default;
  Store(Store&& value);
  Store& operator=(Store&& value);
  ~Store() override = default;

 public:
  bool string_to_data(const std::string& str_data) override;

 public:
  StorageType data_;
#ifdef LABA4
  StorageType* ptr_ = nullptr;
#endif
};

template<IsContainer StorageType>
class MultiValueStore : public BaseStore {
 public:
  MultiValueStore() = default;
  MultiValueStore(const MultiValueStore& value) = default;
  MultiValueStore(MultiValueStore&& value);
  MultiValueStore& operator=(MultiValueStore&& value);
  ~MultiValueStore() override = default;

 public:
  bool string_to_data(const std::string& str_data) override;
#if LABA4
  std::size_t GetCountOfData() const override { return data_.size(); };
#endif

 public:
  StorageType data_;
#ifdef LABA4
  StorageType* ptr_ = nullptr;
#endif
};

template<typename StorageType>
Store<StorageType>::Store(Store&& value) : data_(std::move(value.data_)){  };

template<typename StorageType>
Store<StorageType>& Store<StorageType>::operator=(Store&& value) {
  data_ = value.data_;
  return *this;
};

template<IsContainer StorageType>
MultiValueStore<StorageType>::MultiValueStore(MultiValueStore&& value) : data_(std::move(value.data_)){  };

template<IsContainer StorageType>
MultiValueStore<StorageType>& MultiValueStore<StorageType>::operator=(MultiValueStore&& value) {
  data_ = value.data_;
  return *this;
};

template<typename StorageType>
bool Store<StorageType>::string_to_data(const std::string& str_data) {
  std::stringstream strstr(str_data);

  strstr >> data_;
  if (strstr.fail())
    return false;

#ifdef LABA4
  if (ptr_)
    *ptr_ = data_;
#endif

  return true;
};

template<IsContainer StorageType>
bool MultiValueStore<StorageType>::string_to_data(const std::string& str_data) {
  std::stringstream strstr(str_data);
  typename StorageType::value_type buff;
  strstr >> buff;

  if (strstr.fail())
    return false;

  data_.push_back(buff);

#ifdef LABA4
  if (ptr_)
    *ptr_ = data_;
#endif

  return true;
};

} // argument_parser

#endif // _STORE_HPP_