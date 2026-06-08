#include <cstdlib>
#include <utility>

#include "pulse/core/result_or_die.h"
#include "pulse/http/handler.h"
#include "pulse/http/router.h"
#include "pulse/http/server.h"
#include "vigil/db/accounts_dao.h"
#include "vigil/db/database.h"
#include "vigil/db/transactions_dao.h"
#include "vigil/handlers/create_account_handler.h"
#include "vigil/handlers/create_transaction_handler.h"
#include "vigil/handlers/delete_account_handler.h"
#include "vigil/handlers/get_account_handler.h"
#include "vigil/handlers/health_handler.h"
#include "vigil/handlers/list_transactions_handler.h"

using AccountHandlers =
    pulse::http::Routes<vigil::CreateAccountHandler, vigil::GetAccountHandler,
                        vigil::DeleteAccountHandler>;

using TransactionHandlers = pulse::http::Routes<vigil::CreateTransactionHandler,
                                                vigil::ListTransactionsHandler>;

int main() {
  vigil::Database db = pulse::unwrap_or_die(vigil::Database::Open("vigil.db"));
  pulse::die_if_error(db.Initialize());

  vigil::AccountsDao accounts_dao(db);
  vigil::TransactionsDao transactions_dao(db);

  pulse::http::ServerContext<vigil::AccountsDao*, vigil::TransactionsDao*> ctx;
  ctx.set(&accounts_dao);
  ctx.set(&transactions_dao);

  pulse::http::Router router = pulse::unwrap_or_die(
      pulse::http::Router::Make<pulse::http::Routes<
          vigil::HealthHandler, AccountHandlers, TransactionHandlers>>(ctx));

  pulse::http::Server server(std::move(router), {.port = 8080, .threads = 1});

  server.run();

  return 0;
}
