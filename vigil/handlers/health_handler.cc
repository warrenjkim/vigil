#include "vigil/handlers/health_handler.h"

#include "pulse/http/request.h"
#include "pulse/http/response.h"

namespace vigil {

pulse::http::Response HealthHandler::operator()(
    const pulse::http::Request&) const {
  return pulse::http::Response{.content_type = "application/json",
                               .status = 200,
                               .body = R"({"status": "ok"})"};
}

}  // namespace vigil
