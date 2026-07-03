#pragma once

#include <vector>

#include "pulse/core/result.h"
#include "vigil/db/account.h"
#include "vigil/db/accounts_dao.h"
#include "vigil/db/transactions_dao.h"

namespace vigil {

class AccountService {
 public:
  AccountService(AccountsDao* accounts_dao, TransactionsDao* transactions_dao);

  pulse::Result<std::vector<Account>> ListAccounts();

 private:
  AccountsDao& accounts_dao_;
  TransactionsDao& transactions_dao_;
};

}  // namespace vigil
