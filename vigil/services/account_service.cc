#include "vigil/services/account_service.h"

#include <vector>

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "vigil/db/account.h"
#include "vigil/db/accounts_dao.h"
#include "vigil/db/database.h"
#include "vigil/db/transactions_dao.h"

namespace vigil {

AccountService::AccountService(Database* db, AccountsDao* accounts_dao,
                                 TransactionsDao* transactions_dao)
    : db_(*db),
      accounts_dao_(*accounts_dao),
      transactions_dao_(*transactions_dao) {}

pulse::Result<std::vector<Account>> AccountService::ListAccounts() {
  pulse::Result<std::vector<Account>> accounts = accounts_dao_.ListAccounts();
  if (!accounts.ok()) {
    return accounts.error();
  }

  for (Account& account : *accounts) {
    switch (account.type) {
      case Account::Type::kChecking:
      case Account::Type::kSavings:
      case Account::Type::kCreditCard: {
        pulse::Result<double> balance =
            transactions_dao_.GetBalance(account.name);
        if (!balance.ok()) {
          pulse::Log() << "ListAccounts: balance lookup failed: "
                       << balance.error().message;
          return balance.error();
        }

        account.balance = *balance;
      } break;
      case Account::Type::kBrokerage:
      case Account::Type::k401k:
      case Account::Type::kRothIra:
      case Account::Type::kTraditionalIra:
      case Account::Type::kUnknown:
        break;
    }
  }

  return accounts;
}

}  // namespace vigil
