#include "vigil/handlers/get_account_handler.h"

#include <string>
#include <utility>

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/core/stringify.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/json/value.h"
#include "vigil/db/account.h"
#include "vigil/db/accounts_dao.h"

namespace vigil {

pulse::http::Response GetAccountHandler::operator()(
    const pulse::http::Request& request) const {
  auto name = request.path.get<std::string>("name");
  if (!name.ok()) {
    pulse::Log() << "GetAccount: getting 'name': " << name.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Log() << "GetAccount: handling request (name='" << *name << "')";

  pulse::Result<Account> account = dao_.GetAccount(*name);
  if (!account.ok()) {
    pulse::Log() << "GetAccount: lookup failed: " << account.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 404,
                                 .body = R"({"status": "not found"})"};
  }

  pulse::Log() << "GetAccount: lookup succeeded: "
               << pulse::to_string(*account);

  return pulse::http::Response{.content_type = "application/json",
                               .status = 200,
                               .body = pulse::to_string(pulse::json::object_t{
                                   {"id", account->id},
                                   {"name", std::move(account->name)},
                                   {"type", pulse::to_string(account->type)}})};
}

}  // namespace vigil
