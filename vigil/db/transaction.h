#pragma once

#include <optional>
#include <string>

#include "pulse/core/enum_macros.h"
#include "pulse/core/stringify.h"
#include "pulse/strings/cat.h"
#include "vigil/db/time.h"

namespace vigil {

#define TRANSACTION_TYPE_TABLE(X) \
  X(kDeposit, "DEPOSIT")          \
  X(kWithdrawal, "WITHDRAWAL")

struct Transaction {
  PULSE_ENUM(Type, TRANSACTION_TYPE_TABLE);

  int id;
  std::string account_name;
  Type type;
  double amount;
  std::optional<std::string> description;
  Time timestamp;

  friend bool operator==(const Transaction&, const Transaction&) = default;
};

PULSE_STRING_TO_ENUM(Transaction::Type, to_transfer_type,
                     TRANSACTION_TYPE_TABLE);

}  // namespace vigil

PULSE_ENUM_TO_STRING(vigil::Transaction::Type, TRANSACTION_TYPE_TABLE);

template <>
struct pulse::Stringify<vigil::Transaction> {
  static std::string to_string(const vigil::Transaction& t) {
    return pulse::strings::cat(
        "Transaction{.id=", pulse::to_string(t.id),
        ",.account_name=", pulse::to_string(t.account_name),
        ",.type=", pulse::to_string(t.type),
        ",.amount=", pulse::to_string(t.amount), ",.description=",
        t.description.has_value() ? pulse::to_string(*t.description)
                                  : "std::nullopt",
        ",.timestamp=", pulse::to_string(t.timestamp), "}");
  }
};
