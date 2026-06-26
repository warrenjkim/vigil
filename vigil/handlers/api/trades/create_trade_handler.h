#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/trade_service.h"

namespace vigil {

class CreateTradeHandler final : public pulse::http::Handler {
 public:
  PULSE_HTTP_ROUTE("/accounts/{name}/trades", pulse::http::Method::kPost);
  using Dependencies = pulse::http::Dependencies<TradeService*>;

  explicit CreateTradeHandler(TradeService* service) : service_(*service) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  TradeService& service_;
};

}  // namespace vigil
