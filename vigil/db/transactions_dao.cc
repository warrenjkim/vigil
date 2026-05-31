#include "vigil/db/transactions_dao.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "pulse/core/result.h"
#include "pulse/core/stringify.h"
#include "vigil/db/database.h"
#include "vigil/db/time.h"
#include "vigil/db/transaction.h"

namespace vigil {

TransactionsDao::TransactionsDao(Database db) : db_(db) {}

pulse::Result<void> TransactionsDao::CreateTransaction(
    std::string_view account_name, Transaction::Type type, double amount,
    std::optional<int> transfer_id,
    std::optional<std::string_view> description) {
  return db_.Execute(
      R"sql(
        INSERT INTO Transactions (
          AccountId,
          Type,
          Amount,
          TransferId,
          Description,
          Timestamp
        )
        VALUES (
          (SELECT Id FROM Accounts WHERE Name = :account_name),
          :type,
          :amount,
          :transfer_id,
          :description,
          :timestamp
        )
      )sql",
      /*parameters=*/{{":account_name", account_name},
                      {":type", pulse::to_string(type)},
                      {":amount", amount},
                      {":transfer_id", transfer_id},
                      {":description", description},
                      {":timestamp", Time::Now().ToUnixSeconds()}});
}

pulse::Result<std::vector<Transaction>> TransactionsDao::ListTransactions(
    std::string_view account_name) {
  std::vector<Transaction> transactions;
  if (pulse::Result<void> err = db_.Execute(
          R"sql(
            SELECT
              t.Id,
              a.Name,
              t.Type,
              t.Amount,
              t.Description,
              t.Timestamp
            FROM
              Transactions AS t
            JOIN
              Accounts AS a
            ON
              t.AccountId = a.Id
              AND a.Name = :account_name
          )sql",
          /*parameters=*/{{":account_name", account_name}},
          [&transactions](int id, std::string account_name, std::string type,
                          double amount, std::optional<std::string> description,
                          int64_t timestamp) {
            transactions.push_back(
                Transaction{.id = id,
                            .account_name = account_name,
                            .type = to_transfer_type(type),
                            .amount = amount,
                            .description = std::move(description),
                            .timestamp = Time::FromUnixSeconds(timestamp)});
          });
      !err.ok()) {
    return err.error();
  }

  return transactions;
}

}  // namespace vigil
