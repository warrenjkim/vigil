#include <string.h>

#include <cstdlib>

#include "pulse/core/result_or_die.h"
#include "pulse/http/handler.h"
#include "pulse/http/router.h"
#include "pulse/http/server.h"
#include "vigil/db/accounts_dao.h"
#include "vigil/db/database.h"
#include "vigil/db/holdings_dao.h"
#include "vigil/db/trades_dao.h"
#include "vigil/db/transactions_dao.h"
#include "vigil/handlers/api/accounts/create_account_handler.h"
#include "vigil/handlers/api/accounts/delete_account_handler.h"
#include "vigil/handlers/api/accounts/get_account_handler.h"
#include "vigil/handlers/api/accounts/list_accounts_handler.h"
#include "vigil/handlers/api/holdings/get_holding_handler.h"
#include "vigil/handlers/api/holdings/list_holdings_handler.h"
#include "vigil/handlers/api/trades/create_trade_handler.h"
#include "vigil/handlers/api/trades/list_trades_handler.h"
#include "vigil/handlers/api/transactions/list_transactions_handler.h"
#include "vigil/handlers/pages/dashboard_handler.h"
#include "vigil/handlers/pages/get_new_account_handler.h"
#include "vigil/handlers/pages/health_handler.h"
#include "vigil/handlers/pages/index_handler.h"
#include "vigil/trade_service.h"

// TODO(move to pulse)
const char* GetFlag(int argc, char** argv, const char* flag,
                    const char* default_value) {
  for (int i = 1; i < argc; i++) {
    if (strncmp(argv[i], flag, strlen(flag)) == 0) {
      if (const char* eq = strchr(argv[i], '='); eq != nullptr) {
        return eq + 1;
      }
    }
  }

  return default_value;
}

using StaticHandlers =
    pulse::http::Routes<vigil::IndexHandler, vigil::HealthHandler,
                        vigil::GetNewAccountHandler, vigil::DashboardHandler>;

using AccountHandlers =
    pulse::http::Routes<vigil::CreateAccountHandler, vigil::GetAccountHandler,
                        vigil::DeleteAccountHandler,
                        vigil::ListAccountsHandler>;

using TransactionHandlers = pulse::http::Routes<vigil::ListTransactionsHandler>;

using TradeHandlers =
    pulse::http::Routes<vigil::CreateTradeHandler, vigil::ListTradesHandler>;

using HoldingHandlers =
    pulse::http::Routes<vigil::ListHoldingsHandler, vigil::GetHoldingHandler>;

int main(int argc, char** argv) {
  vigil::Database db = pulse::UnwrapOrDie(
      vigil::Database::Open(GetFlag(argc, argv, "--db", "vigil.db")));
  pulse::DieIfError(db.Initialize());

  vigil::AccountsDao accounts_dao(db);
  vigil::TransactionsDao transactions_dao(db);
  vigil::HoldingsDao holdings_dao(db);
  vigil::TradesDao trades_dao(db);
  vigil::TradeService trade_service(&db, &trades_dao, &holdings_dao);

  pulse::http::ServerContext<vigil::AccountsDao*, vigil::TransactionsDao*,
                             vigil::HoldingsDao*, vigil::TradesDao*,
                             vigil::TradeService*>
      ctx;
  ctx.set(&accounts_dao);
  ctx.set(&transactions_dao);
  ctx.set(&holdings_dao);
  ctx.set(&trades_dao);
  ctx.set(&trade_service);

  pulse::http::Server server(
      pulse::UnwrapOrDie(
          pulse::http::Router::Make<pulse::http::Routes<
              StaticHandlers, AccountHandlers, TransactionHandlers,
              TradeHandlers, HoldingHandlers>>(ctx)),
      {.port = atoi(GetFlag(argc, argv, "--port", "8080")), .threads = 1});
  server.Run();

  return 0;
}
