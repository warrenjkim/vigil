#pragma once

#include <string>

#include "pulse/reflect/mirror.h"

namespace vigil {

struct pulse_reflect Holding {
  int id;
  std::string account_name;
  std::string ticker;
  double shares;

  friend bool operator==(const Holding&, const Holding&) = default;
};

}  // namespace vigil

#include "vigil/db/holding.reflect.h"  // IWYU pragma: keep
