#include <memory>

#include "pulse/http/handler.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"

namespace vigil {

namespace {

struct HealthHandler : pulse::http::Handler {
  pulse::http::Response operator()(const pulse::http::Request&) const override {
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 200,
                                 .body = R"({"status": "ok"})"};
  }
};

}  // namespace

std::unique_ptr<pulse::http::Handler> MakeHealthHandler() {
  return std::make_unique<HealthHandler>();
}

}  // namespace vigil
