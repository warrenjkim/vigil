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
#include "vigil/handlers/create_account_handler.h"
#include "vigil/handlers/create_trade_handler.h"
#include "vigil/handlers/create_transaction_handler.h"
#include "vigil/handlers/delete_account_handler.h"
#include "vigil/handlers/get_account_handler.h"
#include "vigil/handlers/get_holding_handler.h"
#include "vigil/handlers/health_handler.h"
#include "vigil/handlers/list_accounts_handler.h"
#include "vigil/handlers/list_holdings_handler.h"
#include "vigil/handlers/list_trades_handler.h"
#include "vigil/handlers/list_transactions_handler.h"
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

using AccountHandlers =
    pulse::http::Routes<vigil::CreateAccountHandler, vigil::GetAccountHandler,
                        vigil::DeleteAccountHandler,
                        vigil::ListAccountsHandler>;

using TransactionHandlers = pulse::http::Routes<vigil::CreateTransactionHandler,
                                                vigil::ListTransactionsHandler>;

using TradeHandlers =
    pulse::http::Routes<vigil::CreateTradeHandler, vigil::ListTradesHandler>;

using HoldingHandlers =
    pulse::http::Routes<vigil::ListHoldingsHandler, vigil::GetHoldingHandler>;

int main(int argc, char** argv) {
  vigil::Database db = pulse::unwrap_or_die(
      vigil::Database::Open(GetFlag(argc, argv, "--db", "vigil.db")));
  pulse::die_if_error(db.Initialize());

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
      pulse::unwrap_or_die(
          pulse::http::Router::Make<pulse::http::Routes<
              vigil::HealthHandler, AccountHandlers, TransactionHandlers,
              TradeHandlers, HoldingHandlers>>(ctx)),
      {.port = atoi(GetFlag(argc, argv, "--port", "8080")), .threads = 1});
  server.run();

  return 0;
}
