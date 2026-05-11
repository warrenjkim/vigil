#include <memory>

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/server.h"

namespace vigil {

std::unique_ptr<pulse::http::Handler> MakeHealthHandler();

}

int main() {
  pulse::http::Server server({.port = 8080, .threads = 4});
  server.route(pulse::http::Method::kGet, "/health",
               vigil::MakeHealthHandler());
  server.run();

  return 0;
}
