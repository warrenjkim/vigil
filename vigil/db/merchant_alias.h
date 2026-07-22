#pragma once

#include <string>

#include "pulse/core/stringify.h"
#include "pulse/strings/cat.h"

namespace vigil {

struct MerchantAlias {
  std::string merchant;
  std::string display_name;

  friend bool operator==(const MerchantAlias&, const MerchantAlias&) = default;
};

}  // namespace vigil

template <>
struct pulse::Stringify<vigil::MerchantAlias> {
  static std::string ToString(const vigil::MerchantAlias& alias) {
    return pulse::strings::Cat("MerchantAlias{.merchant=", alias.merchant,
                               ",.display_name=", alias.display_name, "}");
  }
};
