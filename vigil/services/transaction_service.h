#pragma once

#include <span>
#include <string_view>

#include "pulse/core/result.h"
#include "vigil/db/accounts_dao.h"
#include "vigil/db/database.h"
#include "vigil/db/transaction.h"
#include "vigil/db/transactions_dao.h"

namespace vigil {

class TransactionService {
 public:
  TransactionService(Database* db, AccountsDao* accounts_dao,
                     TransactionsDao* transactions_dao);

  pulse::Result<int> ImportTransactions(
      std::string_view account_name, std::span<const Transaction> transactions);

 private:
  Database& db_;
  AccountsDao& accounts_dao_;
  TransactionsDao& transactions_dao_;
};

}  // namespace vigil
