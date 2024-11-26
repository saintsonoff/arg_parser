#ifndef _STORE_HPP_
#define _STORE_HPP_

namespace argument_parser {

class BaseStore {
 public:
  virtual ~BaseStore() = 0;
};

template<typename StoringType>
class Store : public BaseStore {
 public:
  Store() = default;
  Store(const Store& value) = default;
  ~Store() override = default;
 private:
  StoringType data_;
};

} // argument_parser

#endif // _STORE_HPP_