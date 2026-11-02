#pragma once

#include <optional>
#include <string>

#include "pulse/core/enum_macros.h"
#include "pulse/reflect/mirror.h"

namespace vigil {

#define ACCOUNT_TYPE_TABLE(X)           \
  X(kChecking, "CHECKING")              \
  X(kSavings, "SAVINGS")                \
  X(kBrokerage, "BROKERAGE")            \
  X(k401k, "401K")                      \
  X(kRothIra, "ROTH_IRA")               \
  X(kTraditionalIra, "TRADITIONAL_IRA") \
  X(kCreditCard, "CREDIT_CARD")

struct pulse_reflect Account {
  PULSE_ENUM(Type, ACCOUNT_TYPE_TABLE);

  int id;
  std::string name;
  Type type;
  std::optional<double> balance;

  friend bool operator==(const Account&, const Account&) = default;
};

PULSE_STRING_TO_ENUM(Account::Type, ToAccountType, ACCOUNT_TYPE_TABLE);

}  // namespace vigil

#include "vigil/db/account.reflect.h"  // IWYU pragma: keep

PULSE_ENUM_TO_STRING(vigil::Account::Type, ACCOUNT_TYPE_TABLE);
