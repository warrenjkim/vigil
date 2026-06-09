#include "vigil/handlers/list_accounts_handler.h"

#include <utility>
#include <vector>

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/core/stringify.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/json/value.h"
#include "vigil/db/account.h"
#include "vigil/db/accounts_dao.h"

namespace vigil {

pulse::http::Response ListAccountsHandler::operator()(
    const pulse::http::Request&) const {
  pulse::Log() << "ListAccounts: handling request";

  pulse::Result<std::vector<Account>> accounts = dao_.ListAccounts();
  if (!accounts.ok()) {
    pulse::Log() << "ListAccounts: list failed: " << accounts.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "ListAccounts: list succeeded (count=" << accounts->size()
               << ", accounts=" << pulse::to_string(*accounts) << ")";

  pulse::json::array_t response_body;
  response_body.reserve(accounts->size());
  for (auto& [id, name, type] : *accounts) {
    response_body.push_back(
        pulse::json::object_t{{"id", id},
                              {"name", std::move(name)},
                              {"type", pulse::to_string(type)}});
  }

  return pulse::http::Response{
      .content_type = "application/json",
      .status = 200,
      .body = pulse::to_string(pulse::json::array_t(std::move(response_body)))};
}

}  // namespace vigil
