#include <cstdlib>
#include <utility>

#include "http/method.h"
#include "http/router.h"
#include "pulse/http/router.h"
#include "pulse/http/server.h"
#include "vigil/db/accounts_dao.h"
#include "vigil/db/database.h"
#include "vigil/handler_registry.h"

int main() {
  // TODO(create a helper function to unwrap or crash)
  vigil::Database db = *vigil::Database::Open("vigil.db");

  vigil::AccountsDao accounts_dao(db);

  pulse::http::Router router;
  if (!router
           .add(pulse::http::Method::kGet, "/health",
                vigil::MakeHealthHandler())
           .ok()) {
    std::exit(1);
  }

  pulse::http::Server server(std::move(router), {.port = 8080, .threads = 1});

  server.run();

  return 0;
}
