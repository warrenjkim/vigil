#pragma once

#include <string>

#include "pulse/reflect/mirror.h"

namespace vigil {

struct pulse_reflect MerchantAlias {
  std::string merchant;
  std::string display_name;

  friend bool operator==(const MerchantAlias&, const MerchantAlias&) = default;
};

}  // namespace vigil

#include "vigil/db/merchant_alias.reflect.h"  // IWYU pragma: keep
