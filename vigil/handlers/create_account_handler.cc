#include "vigil/handlers/create_account_handler.h"

#include <string>

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/account.h"
#include "vigil/db/accounts_dao.h"

namespace vigil {

pulse::http::Response CreateAccountHandler::operator()(
    const pulse::http::Request& request) const {
  auto name = request.query.get<std::string>("name");
  if (!name.ok()) {
    pulse::Log() << "CreateAccount: getting 'name': " << name.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  auto type = request.query.get<std::string>("type");
  if (!type.ok()) {
    pulse::Log() << "CreateAccount: getting 'type': " << type.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  Account::Type account_type = to_account_type(*type);
  if (account_type == Account::Type::kUnknown) {
    pulse::Log() << "CreateAccount: unrecognized type '" << *type << "'";
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Log() << "CreateAccount: handling request (name='" << *name
               << "', type='" << *type << "')";

  if (pulse::Result<void> account = dao_.CreateAccount(*name, account_type);
      !account.ok()) {
    pulse::Log() << "CreateAccount: create failed: " << account.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "CreateAccount: create succeeded (name='" << *name << "')";

  return pulse::http::Response{
      .content_type = "text/plain", .status = 201, .body = "Created"};
}

}  // namespace vigil
