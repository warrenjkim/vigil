#include "pulse/http/method.h"
#include "pulse/http/server.h"
#include "vigil/handler_registry.h"

int main() {
  pulse::http::Server server({.port = 8080, .threads = 4});
  server.route(pulse::http::Method::kGet, "/health",
               vigil::MakeHealthHandler());
  server.run();

  return 0;
}
