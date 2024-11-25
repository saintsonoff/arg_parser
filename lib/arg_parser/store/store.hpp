#ifndef _STORE_HPP_
#define _STORE_HPP_

namespace argument_parser {

class BaseStore {
  virtual ~BaseStore() = 0;
};

template<typename StoringType>
class Store : public BaseStore {
  StoringType data_;
  ~Store() override;
};

} // argument_parser

#endif // _STORE_HPP_