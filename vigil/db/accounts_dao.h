#pragma once

#include <string_view>

#include "pulse/core/result.h"
#include "vigil/db/account.h"
#include "vigil/db/database.h"

namespace vigil {

class AccountsDao {
 public:
  explicit AccountsDao(Database db);

  pulse::Result<void> CreateAccount(std::string_view name, Account::Type type);

  pulse::Result<Account> GetAccount(std::string_view name);

  pulse::Result<void> DeleteAccount(std::string_view name);

 private:
  Database db_;
};

}  // namespace vigil
