#include "store.hpp"
#include <argument.hpp>
#include <iterator>
#include <memory>

#include <lexer/lexer.hpp>

namespace argument_parser {

Argument::Argument(std::string_view full_name,
  std::string_view short_name, std::string_view descriprion) :
    full_name_(full_name), short_name_(short_name), descriprion_(descriprion),
    store_(nullptr), is_found_(FoundClasses::NOT_FOUND) {  };

Argument::Argument(std::string_view full_name, std::string_view descriprion) :
  Argument(full_name, "", descriprion) {  };

Argument::Argument(std::string_view full_name) :
  Argument(full_name, "", "") {  };

Argument::Argument(Argument&& value) :
  full_name_(value.full_name_), short_name_(value.short_name_), descriprion_(value.descriprion_),
  is_multivalue_(value.is_multivalue_), is_positional_(value.is_positional_), is_found_(value.is_found_),
  min_val(value.min_val), store_(std::move(value.store_)) {  };

Argument& Argument::operator=(Argument&& value) {
  if (this == &value)
    return *this;

  full_name_ = value.full_name_;
  short_name_ = value.short_name_;
  descriprion_ = value.descriprion_;
  is_multivalue_ = value.is_multivalue_;
  is_positional_ = value.is_positional_;
  store_ = std::move(value.store_);

  return *this;
}

bool Argument::convert(std::string_view string_data) {
  is_found_ = FoundClasses::WAS_INITIALIZE;
  return store_->string_to_data({string_data.begin(), string_data.end()});
};

template<>
Argument& Argument::SetStore<bool>(Store<bool>* store_ptr) {
  if (store_) {
    store_.reset();
  }
  store_ = std::move(std::unique_ptr<Store<bool>>(store_ptr));
  WasFound();
  return *this;
};

} // argument_parser