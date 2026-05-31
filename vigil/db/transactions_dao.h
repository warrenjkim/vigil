#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "pulse/core/result.h"
#include "vigil/db/database.h"
#include "vigil/db/transaction.h"

namespace vigil {

class TransactionsDao {
 public:
  explicit TransactionsDao(Database db);

  pulse::Result<void> CreateTransaction(
      std::string_view account_name, Transaction::Type type, double amount,
      std::optional<int> transfer_id = std::nullopt,
      std::optional<std::string_view> description = std::nullopt);

  pulse::Result<std::vector<Transaction>> ListTransactions(
      std::string_view account_id);

 private:
  Database db_;
};

}  // namespace vigil
