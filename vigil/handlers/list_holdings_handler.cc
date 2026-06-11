#include "vigil/handlers/list_holdings_handler.h"

#include <string>
#include <utility>
#include <vector>

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/core/stringify.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/json/value.h"
#include "vigil/db/holding.h"
#include "vigil/db/holdings_dao.h"

namespace vigil {

pulse::http::Response ListHoldingsHandler::operator()(
    const pulse::http::Request& request) const {
  auto account_name = request.path.get<std::string>("name");
  if (!account_name.ok()) {
    pulse::Log() << "ListHoldings: getting 'name': "
                 << account_name.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Log() << "ListHoldings: handling request (name='" << *account_name
               << "')";

  pulse::Result<std::vector<Holding>> holdings =
      dao_.ListHoldings(*account_name);
  if (!holdings.ok()) {
    pulse::Log() << "ListHoldings: list failed: " << holdings.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "ListHoldings: list succeeded (name='" << *account_name
               << "', count=" << holdings->size()
               << ", holdings=" << pulse::to_string(*holdings) << ")";

  pulse::json::array_t response_body;
  response_body.reserve(holdings->size());
  for (auto& [id, account_name, ticker, shares] : *holdings) {
    response_body.push_back(
        pulse::json::object_t{{"id", id},
                              {"account_name", std::move(account_name)},
                              {"ticker", std::move(ticker)},
                              {"shares", shares}});
  }

  return pulse::http::Response{.content_type = "application/json",
                               .status = 200,
                               .body = pulse::to_string(response_body)};
}

}  // namespace vigil
