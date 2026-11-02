#pragma once

#include <optional>
#include <string>

#include "pulse/core/enum_macros.h"
#include "pulse/reflect/mirror.h"
#include "vigil/db/time.h"

namespace vigil {

#define TRADE_TYPE_TABLE(X) \
  X(kBuy, "BUY")            \
  X(kSell, "SELL")

struct pulse_reflect Trade {
  PULSE_ENUM(Type, TRADE_TYPE_TABLE);

  int id;
  std::string account_name;
  Type type;
  std::string ticker;
  double shares;
  double price;
  std::optional<std::string> description;
  Time trade_timestamp;

  friend bool operator==(const Trade&, const Trade&) = default;
};

PULSE_STRING_TO_ENUM(Trade::Type, to_trade_type, TRADE_TYPE_TABLE);

}  // namespace vigil

#include "vigil/db/trade.reflect.h"  // IWYU pragma: keep

PULSE_ENUM_TO_STRING(vigil::Trade::Type, TRADE_TYPE_TABLE);
