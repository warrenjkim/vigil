#include "vigil/handlers/api/holdings/get_holding_handler.h"

#include <string>
#include <utility>

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/core/stringify.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/json/value.h"
#include "vigil/db/holding.h"
#include "vigil/db/holdings_dao.h"

namespace vigil {

pulse::http::Response GetHoldingHandler::operator()(
    const pulse::http::Request& request) const {
  auto account_name = request.path.Get<std::string>("name");
  if (!account_name.ok()) {
    pulse::Log() << "GetHolding: getting 'name': "
                 << account_name.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  auto ticker = request.path.Get<std::string>("ticker");
  if (!ticker.ok()) {
    pulse::Log() << "GetHolding: getting 'ticker': " << ticker.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Log() << "GetHolding: handling request (name='" << *account_name
               << "', ticker='" << *ticker << "')";

  pulse::Result<Holding> holding = dao_.GetHolding(*account_name, *ticker);
  if (!holding.ok()) {
    pulse::Log() << "GetHolding: lookup failed: " << holding.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 404,
                                 .body = R"({"status": "not found"})"};
  }

  pulse::Log() << "GetHolding: lookup succeeded: " << pulse::ToString(*holding);

  return pulse::http::Response{
      .content_type = "application/json",
      .status = 200,
      .body = pulse::ToString(pulse::json::Object{
          {"id", holding->id},
          {"account_name", std::move(holding->account_name)},
          {"ticker", std::move(holding->ticker)},
          {"shares", holding->shares}})};
}

}  // namespace vigil
