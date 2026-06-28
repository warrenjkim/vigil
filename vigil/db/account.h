#pragma once

#include <string>

#include "pulse/core/enum_macros.h"
#include "pulse/core/stringify.h"
#include "pulse/strings/cat.h"

namespace vigil {

#define ACCOUNT_TYPE_TABLE(X)           \
  X(kChecking, "CHECKING")              \
  X(kSavings, "SAVINGS")                \
  X(kBrokerage, "BROKERAGE")            \
  X(k401k, "401K")                      \
  X(kRothIra, "ROTH_IRA")               \
  X(kTraditionalIra, "TRADITIONAL_IRA") \
  X(kCreditCard, "CREDIT_CARD")

struct Account {
  PULSE_ENUM(Type, ACCOUNT_TYPE_TABLE);

  int id;
  std::string name;
  Type type;

  friend bool operator==(const Account&, const Account&) = default;
};

PULSE_STRING_TO_ENUM(Account::Type, ToAccountType, ACCOUNT_TYPE_TABLE);

}  // namespace vigil

PULSE_ENUM_TO_STRING(vigil::Account::Type, ACCOUNT_TYPE_TABLE);

template <>
struct pulse::Stringify<vigil::Account> {
  static std::string ToString(const vigil::Account& account) {
    return pulse::strings::Cat("Account{.id=", account.id,
                               ",.name=", account.name, ",.type=", account.type,
                               "}");
  }
};
