#pragma once

#include <string>

#include "pulse/core/enum_macros.h"
#include "pulse/reflect/mirror.h"
#include "vigil/db/time.h"

namespace vigil {

#define TRANSACTION_TYPE_TABLE(X) \
  X(kDeposit, "DEPOSIT")          \
  X(kWithdrawal, "WITHDRAWAL")

struct pulse_reflect Transaction {
  PULSE_ENUM(Type, TRANSACTION_TYPE_TABLE);

  int id;
  std::string external_id;
  std::string account_name;
  Type type;
  double amount;
  std::string merchant;
  Time transaction_timestamp;

  friend bool operator==(const Transaction&, const Transaction&) = default;
};

PULSE_STRING_TO_ENUM(Transaction::Type, ToTransactionType,
                     TRANSACTION_TYPE_TABLE);

}  // namespace vigil

#include "vigil/db/transaction.reflect.h"  // IWYU pragma: keep

PULSE_ENUM_TO_STRING(vigil::Transaction::Type, TRANSACTION_TYPE_TABLE);
