#include <cstdlib>
#include <utility>

#include "http/method.h"
#include "http/router.h"
#include "pulse/core/result_or_die.h"
#include "pulse/http/router.h"
#include "pulse/http/server.h"
#include "vigil/db/accounts_dao.h"
#include "vigil/db/database.h"
#include "vigil/handlers/handler_registry.h"

int main() {
  vigil::Database db = pulse::unwrap_or_die(vigil::Database::Open("vigil.db"));
  pulse::die_if_error(db.Initialize());

  vigil::AccountsDao accounts_dao(db);

  pulse::http::Router router;
  pulse::die_if_error(router.add(pulse::http::Method::kGet, "/health",
                                 vigil::MakeHealthHandler()));
  pulse::die_if_error(router.add(pulse::http::Method::kGet, "/accounts/{name}",
                                 vigil::MakeGetAccountHandler(&accounts_dao)));
  pulse::die_if_error(
      router.add(pulse::http::Method::kPost, "/accounts",
                 vigil::MakeCreateAccountHandler(&accounts_dao)));

  pulse::http::Server server(std::move(router), {.port = 8080, .threads = 1});

  server.run();

  return 0;
}
