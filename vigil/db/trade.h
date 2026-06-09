#pragma once

#include <optional>
#include <string>

#include "pulse/core/enum_macros.h"
#include "pulse/core/stringify.h"
#include "pulse/strings/cat.h"
#include "vigil/db/time.h"

namespace vigil {

#define TRADE_TYPE_TABLE(X) \
  X(kBuy, "BUY")            \
  X(kSell, "SELL")

struct Trade {
  PULSE_ENUM(Type, TRADE_TYPE_TABLE);

  int id;
  std::string account_name;
  Type type;
  std::string ticker;
  double shares;
  double price;
  std::optional<std::string> description;
  Time timestamp;

  friend bool operator==(const Trade&, const Trade&) = default;
};

PULSE_STRING_TO_ENUM(Trade::Type, to_trade_type, TRADE_TYPE_TABLE);

}  // namespace vigil

PULSE_ENUM_TO_STRING(vigil::Trade::Type, TRADE_TYPE_TABLE);

template <>
struct pulse::Stringify<vigil::Trade> {
  static std::string to_string(const vigil::Trade& t) {
    return pulse::strings::cat(
        "Trade{.id=", t.id, ",.account_name=", t.account_name,
        ",.type=", t.type, ",.ticker=", t.ticker, ",.shares=", t.shares,
        ",.price=", t.price, ",.description=",
        t.description.has_value() ? *t.description : "std::nullopt",
        ",.timestamp=", t.timestamp, "}");
  }
};
