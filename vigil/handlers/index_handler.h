#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"

namespace vigil {

class IndexHandler final : public pulse::http::Handler {
 public:
  PULSE_HTTP_ROUTE("/", pulse::http::Method::kGet);
  using Dependencies = pulse::http::Dependencies<>;

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;
};

}  // namespace vigil
