#include "vigil/handlers/api/trades/create_trade_handler.h"

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

#include "pulse/core/error.h"
#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/json/bind.h"
#include "pulse/json/parse.h"
#include "pulse/json/value.h"
#include "vigil/db/time.h"
#include "vigil/db/trade.h"
#include "vigil/services/trade_service.h"

namespace vigil {

namespace {

struct CreateTradeRequest {
  std::string type;
  std::string ticker;
  double shares;
  double price;
  std::optional<std::string> description;
  int64_t trade_timestamp;

  static constexpr auto schema() {
    return pulse::json::Schema<CreateTradeRequest>{}
        .Field("type", &CreateTradeRequest::type)
        .Field("ticker", &CreateTradeRequest::ticker)
        .Field("shares", &CreateTradeRequest::shares)
        .Field("price", &CreateTradeRequest::price)
        .Field("description", &CreateTradeRequest::description)
        .Field("trade_timestamp", &CreateTradeRequest::trade_timestamp);
  }
};

}  // namespace

pulse::http::Response CreateTradeHandler::operator()(
    const pulse::http::Request& request) const {
  auto account_name = request.path.Get<std::string>("name");
  if (!account_name.ok()) {
    pulse::Log() << "CreateTrade: getting 'name': "
                 << account_name.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Result<pulse::json::Value> body = pulse::json::Parse(request.body);
  if (!body.ok()) {
    pulse::Log() << "CreateTrade: parsing body: " << body.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  auto req = pulse::json::Bind<CreateTradeRequest>(*std::move(body));
  if (!req.ok()) {
    pulse::Log() << "CreateTrade: binding body: " << req.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  Trade::Type trade_type = to_trade_type(req->type);
  if (trade_type == Trade::Type::kUnknown) {
    pulse::Log() << "CreateTrade: unrecognized type '" << req->type << "'";
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Log() << "CreateTrade: handling request (name='" << *account_name
               << "', type='" << req->type << "', ticker='" << req->ticker
               << "', shares=" << req->shares << ", price=" << req->price
               << ")";

  if (pulse::Result<void> result = service_.RecordTrade(
          *account_name, trade_type, req->ticker, req->shares, req->price,
          req->description, Time::FromUnixSeconds(req->trade_timestamp));
      !result.ok()) {
    pulse::Log() << "CreateTrade: record failed: " << result.error().message;
    if (result.error().code == pulse::Error::Code::kFailedPrecondition) {
      return pulse::http::Response{
          .content_type = "application/json",
          .status = 422,
          .body = R"({"status": "unprocessable entity"})"};
    }

    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "CreateTrade: record succeeded (name='" << *account_name
               << "')";

  return pulse::http::Response{
      .content_type = "text/plain", .status = 201, .body = "Created"};
}

}  // namespace vigil
