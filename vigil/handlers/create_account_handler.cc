#include "vigil/handlers/create_account_handler.h"

#include <string>
#include <utility>

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/json/bind.h"
#include "pulse/json/parse.h"
#include "pulse/json/value.h"
#include "vigil/db/account.h"
#include "vigil/db/accounts_dao.h"

namespace vigil {

namespace {

struct CreateAccountRequest {
  std::string name;
  std::string type;

  static constexpr auto schema() {
    return pulse::json::Schema<CreateAccountRequest>{}
        .Field("name", &CreateAccountRequest::name)
        .Field("type", &CreateAccountRequest::type);
  }
};

}  // namespace

pulse::http::Response CreateAccountHandler::operator()(
    const pulse::http::Request& request) const {
  pulse::Result<pulse::json::Value> body = pulse::json::Parse(request.body);
  if (!body.ok()) {
    pulse::Log() << "CreateAccount: parsing body: " << body.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  auto req = pulse::json::Bind<CreateAccountRequest>(std::move(*body));
  if (!req.ok()) {
    pulse::Log() << "CreateAccount: binding body: " << req.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  Account::Type account_type = ToAccountType(req->type);
  if (account_type == Account::Type::kUnknown) {
    pulse::Log() << "CreateAccount: unrecognized type '" << req->type << "'";
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Log() << "CreateAccount: handling request (name='" << req->name
               << "', type='" << req->type << "')";

  if (pulse::Result<void> account = dao_.CreateAccount(req->name, account_type);
      !account.ok()) {
    pulse::Log() << "CreateAccount: create failed: " << account.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "CreateAccount: create succeeded (name='" << req->name
               << "')";

  return pulse::http::Response{
      .content_type = "text/plain", .status = 201, .body = "Created"};
}

}  // namespace vigil
