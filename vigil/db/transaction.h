#pragma once

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
  std::string merchant;
  Time transaction_timestamp;

  friend bool operator==(const Transaction&, const Transaction&) = default;
};

PULSE_STRING_TO_ENUM(Transaction::Type, ToTransferType, TRANSACTION_TYPE_TABLE);

}  // namespace vigil

PULSE_ENUM_TO_STRING(vigil::Transaction::Type, TRANSACTION_TYPE_TABLE);

template <>
struct pulse::Stringify<vigil::Transaction> {
  static std::string ToString(const vigil::Transaction& t) {
    return pulse::strings::Cat(
        "Transaction{.id=", t.id, ",.account_name=", t.account_name,
        ",.type=", t.type, ",.amount=", t.amount, ",.merchant=", t.merchant,
        ",.timestamp=", t.transaction_timestamp, "}");
  }
};
