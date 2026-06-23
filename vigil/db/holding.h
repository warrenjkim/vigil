#pragma once

#include <string>

#include "pulse/core/stringify.h"
#include "pulse/strings/cat.h"

namespace vigil {

struct Holding {
  int id;
  std::string account_name;
  std::string ticker;
  double shares;

  friend bool operator==(const Holding&, const Holding&) = default;
};

}  // namespace vigil

template <>
struct pulse::Stringify<vigil::Holding> {
  static std::string ToString(const vigil::Holding& h) {
    return pulse::strings::Cat(
        "Holding{.id=", h.id, ",.account_name=", h.account_name,
        ",.ticker=", h.ticker, ",.shares=", h.shares, "}");
  }
};
