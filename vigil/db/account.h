#pragma once

#include <string>

#include "pulse/core/enum_macros.h"
#include "pulse/core/stringify.h"

namespace vigil {

#define ACCOUNT_TYPE_TABLE(X) \
  X(kChecking, "CHECKING")    \
  X(kSavings, "SAVINGS")      \
  X(kBrokerage, "BROKERAGE")  \
  X(k401k, "401K")            \
  X(kRothIra, "ROTH_IRA")     \
  X(kTraditionalIra, "TRADITIONAL_IRA")

struct Account {
  PULSE_ENUM(Type, ACCOUNT_TYPE_TABLE);

  int id;
  std::string name;
  Type type;

  friend bool operator==(const Account&, const Account&) = default;
};

PULSE_STRING_TO_ENUM(Account::Type, to_account_type, ACCOUNT_TYPE_TABLE);

}  // namespace vigil

PULSE_ENUM_TO_STRING(vigil::Account::Type, ACCOUNT_TYPE_TABLE);

template <>
struct pulse::Stringify<vigil::Account> {
  static std::string to_string(const vigil::Account& account) {
    return "Account{.id = " + std::to_string(account.id) +
           ", .name = " + account.name + ", .type = " +
           pulse::Stringify<vigil::Account::Type>::to_string(account.type) +
           "}";
  }
};
