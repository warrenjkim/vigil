#pragma once

#include <string_view>
#include <vector>

#include "pulse/core/result.h"
#include "vigil/db/database.h"
#include "vigil/db/time.h"
#include "vigil/db/transaction.h"

namespace vigil {

class TransactionsDao {
 public:
  explicit TransactionsDao(Database db);

  pulse::Result<bool> CreateTransaction(std::string_view account_name,
                                        std::string_view external_id,
                                        Transaction::Type type, double amount,
                                        std::string_view merchant,
                                        Time transaction_timestamp);

  pulse::Result<std::vector<Transaction>> ListTransactions(
      std::string_view account_name);

  pulse::Result<double> GetBalance(std::string_view account_name);

 private:
  Database db_;
};

}  // namespace vigil
