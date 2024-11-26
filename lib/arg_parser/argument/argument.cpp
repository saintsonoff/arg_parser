#include <argument.hpp>
#include <memory>

namespace argument_parser {

Argument::Argument(std::string_view full_name,
  std::string_view short_name, std::string_view descriprion) :
     full_name_(full_name), short_name_(short_name), descriprion_(descriprion), store_(nullptr) {  };

Argument::Argument(std::string_view full_name, std::string_view descriprion) :
  Argument(full_name, "", descriprion) {  };

Argument::Argument(std::string_view full_name) :
  Argument(full_name, "", "") {  };

Argument::Argument(Argument&& value) :
  full_name_(value.full_name_), short_name_(value.short_name_), descriprion_(value.descriprion_),
  is_multivalue_(value.is_multivalue_), is_positional_(value.is_positional_),
  store_(std::move(value.store_)) {  };

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

} // argument_parser