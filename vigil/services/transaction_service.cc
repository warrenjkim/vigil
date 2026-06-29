#include "vigil/services/transaction_service.h"

#include <span>
#include <string_view>

#include "pulse/core/result.h"
#include "vigil/db/account.h"
#include "vigil/db/accounts_dao.h"
#include "vigil/db/database.h"
#include "vigil/db/transaction.h"
#include "vigil/db/transactions_dao.h"

namespace vigil {

TransactionService::TransactionService(Database* db, AccountsDao* accounts_dao,
                                       TransactionsDao* transactions_dao)
    : db_(*db),
      accounts_dao_(*accounts_dao),
      transactions_dao_(*transactions_dao) {}

pulse::Result<int> TransactionService::ImportTransactions(
    std::string_view account_name, std::span<const Transaction> transactions) {
  if (pulse::Result<Account> account = accounts_dao_.GetAccount(account_name);
      !account.ok()) {
    return account.error();
  }

  int imported = 0;
  if (pulse::Result<void> err = WrapInTransaction(
          &db_,
          [this, &imported, account_name, transactions] -> pulse::Result<void> {
            for (const Transaction& t : transactions) {
              pulse::Result<bool> created = transactions_dao_.CreateTransaction(
                  account_name, t.external_id, t.type, t.amount, t.merchant,
                  t.transaction_timestamp);
              if (!created.ok()) {
                return created.error();
              }

              imported += *created;
            }

            return pulse::Result<void>{};
          });
      !err.ok()) {
    return err.error();
  }

  return imported;
}

}  // namespace vigil
