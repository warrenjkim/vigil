#include "vigil/handlers/api/trades/list_trades_handler.h"

#include <string>
#include <utility>
#include <vector>

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/core/stringify.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/json/value.h"
#include "vigil/db/trade.h"
#include "vigil/db/trades_dao.h"

namespace vigil {

pulse::http::Response ListTradesHandler::operator()(
    const pulse::http::Request& request) const {
  auto account_name = request.path.Get<std::string>("name");
  if (!account_name.ok()) {
    pulse::Log() << "ListTrades: getting 'name': "
                 << account_name.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Log() << "ListTrades: handling request (name='" << *account_name
               << "')";

  pulse::Result<std::vector<Trade>> trades = dao_.ListTrades(*account_name);
  if (!trades.ok()) {
    pulse::Log() << "ListTrades: list failed: " << trades.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "ListTrades: list succeeded (name='" << *account_name
               << "', count=" << trades->size()
               << ", trades=" << pulse::ToString(*trades) << ")";

  pulse::json::Array response_body;
  response_body.reserve(trades->size());
  for (auto& [id, account_name, type, ticker, shares, price, description,
              timestamp] : *trades) {
    response_body.push_back(pulse::json::Object{
        {"id", id},
        {"account_name", std::move(account_name)},
        {"type", pulse::ToString(type)},
        {"ticker", std::move(ticker)},
        {"shares", shares},
        {"price", price},
        {"description", description.has_value() ? *std::move(description)
                                                : pulse::json::Value{nullptr}},
        {"timestamp", pulse::ToString(timestamp)}});
  }

  return pulse::http::Response{.content_type = "application/json",
                               .status = 200,
                               .body = pulse::ToString(response_body)};
}

}  // namespace vigil
