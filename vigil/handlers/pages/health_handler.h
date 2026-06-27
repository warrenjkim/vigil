#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"

namespace vigil {

struct HealthHandler
    : pulse::http::Handler<pulse::http::Method::kGet, "/health"> {
  pulse::http::Response operator()(const pulse::http::Request&) const override;
};

}  // namespace vigil
