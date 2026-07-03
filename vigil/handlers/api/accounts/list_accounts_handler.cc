#include "vigil/handlers/api/accounts/list_accounts_handler.h"

#include <utility>
#include <vector>

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/core/stringify.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/json/value.h"
#include "vigil/db/account.h"
#include "vigil/services/account_service.h"

namespace vigil {

pulse::http::Response ListAccountsHandler::operator()(
    const pulse::http::Request&) const {
  pulse::Log() << "ListAccounts: handling request";

  pulse::Result<std::vector<Account>> accounts = service_.ListAccounts();
  if (!accounts.ok()) {
    pulse::Log() << "ListAccounts: list failed: " << accounts.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "ListAccounts: list succeeded (count=" << accounts->size()
               << ", accounts=" << pulse::ToString(*accounts) << ")";

  pulse::json::Array response_body;
  response_body.reserve(accounts->size());
  for (auto& [id, name, type, balance] : *accounts) {
    pulse::json::Object account = {
        {"id", id}, {"name", std::move(name)}, {"type", pulse::ToString(type)}};
    if (balance.has_value()) {
      account["balance"] = *balance;
    }

    response_body.push_back(std::move(account));
  }

  return pulse::http::Response{
      .content_type = "application/json",
      .status = 200,
      .body = pulse::ToString(std::move(response_body))};
}

}  // namespace vigil
