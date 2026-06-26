#include "vigil/handlers/api/accounts/create_account_handler.h"

#include <string>
#include <utility>

#include "pulse/core/error.h"
#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/http/parameters.h"
#include "pulse/http/parse_form.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/json/bind.h"
#include "pulse/json/parse.h"
#include "pulse/json/value.h"
#include "pulse/strings/cat.h"
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

struct Parsed {
  bool is_form = false;
  CreateAccountRequest request;
};

pulse::Result<Parsed> ParseRequest(const pulse::http::Request& request) {
  if (const auto it = request.headers.find("Content-Type");
      it != request.headers.end() &&
      it->second == "application/x-www-form-urlencoded") {
    pulse::Result<pulse::http::Parameters> form =
        pulse::http::ParseForm(request.body);
    if (!form.ok()) {
      pulse::Log() << "CreateAccount: parsing form: " << form.error().message;
      return form.error();
    }

    pulse::Result<std::string> name = form->Get<std::string>("name");
    pulse::Result<std::string> type = form->Get<std::string>("type");
    if (!name.ok() || !type.ok()) {
      pulse::Log() << "CreateAccount: missing form fields (name="
                   << (name.ok() ? "ok" : "missing")
                   << ", type=" << (type.ok() ? "ok" : "missing") << ")";
      return pulse::Error{.code = pulse::Error::Code::kInvalidArgument,
                          .message = "missing form fields"};
    }

    pulse::Log() << "CreateAccount: parsed form (name='" << *name << "', type='"
                 << *type << "')";
    return Parsed{.is_form = true,
                  .request = CreateAccountRequest{.name = *std::move(name),
                                                  .type = *std::move(type)}};
  }

  pulse::Result<pulse::json::Value> body = pulse::json::Parse(request.body);
  if (!body.ok()) {
    pulse::Log() << "CreateAccount: parsing body: " << body.error().message;
    return body.error();
  }

  pulse::Result<CreateAccountRequest> req =
      pulse::json::Bind<CreateAccountRequest>(*std::move(body));
  if (!req.ok()) {
    pulse::Log() << "CreateAccount: binding body: " << req.error().message;
    return req.error();
  }

  pulse::Log() << "CreateAccount: parsed json (name='" << req->name
               << "', type='" << req->type << "')";
  return Parsed{.is_form = false, .request = *std::move(req)};
}

}  // namespace

pulse::http::Response CreateAccountHandler::operator()(
    const pulse::http::Request& request) const {
  pulse::Result<Parsed> parsed = ParseRequest(request);
  if (!parsed.ok()) {
    pulse::Log() << "CreateAccount: parsing request: "
                 << parsed.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  const auto [is_form, req] = *std::move(parsed);
  Account::Type account_type = ToAccountType(req.type);
  if (account_type == Account::Type::kUnknown) {
    pulse::Log() << "CreateAccount: unrecognized type '" << req.type << "'";
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Log() << "CreateAccount: handling request (name='" << req.name
               << "', type='" << req.type << "')";

  if (pulse::Result<void> account = dao_.CreateAccount(req.name, account_type);
      !account.ok()) {
    pulse::Log() << "CreateAccount: create failed: " << account.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "CreateAccount: create succeeded (name='" << req.name << "')";

  return is_form
             ? pulse::http::Response{.headers = {{"Location",
                                                  pulse::strings::Cat(
                                                      "/accounts/", req.name)}},
                                     .status = 303}
             : pulse::http::Response{.content_type = "text/plain",
                                     .status = 201,
                                     .body = "Created"};
}

}  // namespace vigil
