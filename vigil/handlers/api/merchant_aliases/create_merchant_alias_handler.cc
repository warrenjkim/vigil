#include "vigil/handlers/api/merchant_aliases/create_merchant_alias_handler.h"

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
#include "pulse/reflect/schema.h"
#include "vigil/db/merchant_aliases_dao.h"

namespace vigil {

namespace {

struct CreateMerchantAliasRequest {
  std::string merchant;
  std::string display_name;

  static constexpr auto Schema() {
    return pulse::reflect::Schema<CreateMerchantAliasRequest>{}
        .Field("merchant", &CreateMerchantAliasRequest::merchant)
        .Field("display_name", &CreateMerchantAliasRequest::display_name);
  }
};

}  // namespace

pulse::http::Response CreateMerchantAliasHandler::operator()(
    const pulse::http::Request& request) const {
  pulse::Result<pulse::json::Value> body = pulse::json::Parse(request.body);
  if (!body.ok()) {
    pulse::Log() << "CreateMerchantAlias: parsing body: "
                 << body.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  auto req = pulse::json::Bind<CreateMerchantAliasRequest>(*std::move(body));
  if (!req.ok()) {
    pulse::Log() << "CreateMerchantAlias: binding body: "
                 << req.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Log() << "CreateMerchantAlias: handling request (merchant='"
               << req->merchant << "', display_name='" << req->display_name
               << "')";

  if (pulse::Result<void> err =
          dao_.CreateAlias(req->merchant, req->display_name);
      !err.ok()) {
    pulse::Log() << "CreateMerchantAlias: create failed: "
                 << err.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "CreateMerchantAlias: create succeeded (merchant='"
               << req->merchant << "')";

  return pulse::http::Response{
      .content_type = "text/plain", .status = 201, .body = "Created"};
}

}  // namespace vigil
