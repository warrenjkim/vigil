#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/reflect/schema.h"
#include "vigil/services/trade_service.h"

namespace vigil {

struct CreateTradeRequest {
  std::string type;
  std::string ticker;
  double shares;
  double price;
  std::optional<std::string> description;
  int64_t trade_timestamp;

  static constexpr auto Schema() {
    return pulse::reflect::Schema<CreateTradeRequest>{}
        .Field("type", &CreateTradeRequest::type)
        .Field("ticker", &CreateTradeRequest::ticker)
        .Field("shares", &CreateTradeRequest::shares)
        .Field("price", &CreateTradeRequest::price)
        .Field("description", &CreateTradeRequest::description)
        .Field("trade_timestamp", &CreateTradeRequest::trade_timestamp);
  }
};

class CreateTradeHandler final
    : public pulse::http::Handler<
          pulse::http::Method::kPost, "/accounts/{name}/trades",
          pulse::http::Dependencies<TradeService*>, CreateTradeRequest> {
 public:
  explicit CreateTradeHandler(TradeService* service) : service_(*service) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  TradeService& service_;
};

}  // namespace vigil
