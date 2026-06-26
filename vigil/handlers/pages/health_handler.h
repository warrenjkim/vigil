#pragma once

#include <string_view>

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"

namespace vigil {

struct HealthHandler : pulse::http::Handler {
  PULSE_HTTP_ROUTE("/health", pulse::http::Method::kGet);
  using Dependencies = pulse::http::Dependencies<>;

  pulse::http::Response operator()(const pulse::http::Request&) const override;
};

}  // namespace vigil
