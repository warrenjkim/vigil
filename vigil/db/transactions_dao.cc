#include "vigil/db/transactions_dao.h"

#include <cstdint>
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
    std::string_view merchant, Time transaction_timestamp) {
  return db_.Execute(
      R"sql(
        INSERT INTO Transactions (
          AccountId,
          Type,
          Amount,
          Merchant,
          TransactionTimestamp,
          CommitTimestamp
        )
        VALUES (
          (SELECT Id FROM Accounts WHERE Name = :account_name),
          :type,
          :amount,
          :merchant,
          :transaction_timestamp,
          :commit_timestamp
        )
      )sql",
      /*parameters=*/{
          {":account_name", account_name},
          {":type", pulse::ToString(type)},
          {":amount", amount},
          {":merchant", merchant},
          {":transaction_timestamp", transaction_timestamp.ToUnixSeconds()},
          {":commit_timestamp", Time::Now().ToUnixSeconds()}});
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
              t.Merchant,
              t.TransactionTimestamp
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
                          double amount, std::string merchant,
                          int64_t transaction_timestamp) {
            transactions.push_back(
                Transaction{.id = id,
                            .account_name = std::move(account_name),
                            .type = ToTransferType(type),
                            .amount = amount,
                            .merchant = std::move(merchant),
                            .transaction_timestamp =
                                Time::FromUnixSeconds(transaction_timestamp)});
          });
      !err.ok()) {
    return err.error();
  }

  return transactions;
}
pulse::Result<double> TransactionsDao::GetBalance(
    std::string_view account_name) {
  double balance = 0.0;
  if (pulse::Result<void> err = db_.Execute(
          R"sql(
            SELECT
              COALESCE(SUM(CASE WHEN Type = 'DEPOSIT' THEN Amount ELSE -Amount END), 0.0)
            FROM
              Transactions
            WHERE
              AccountId = (SELECT Id FROM Accounts WHERE Name = :account_name)
          )sql",
          /*parameters=*/{{":account_name", account_name}},
          [&balance](double b) { balance = b; });
      !err.ok()) {
    return err.error();
  }

  return balance;
}

}  // namespace vigil
