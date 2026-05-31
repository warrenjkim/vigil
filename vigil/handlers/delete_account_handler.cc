#include "vigil/handlers/delete_account_handler.h"

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/accounts_dao.h"

namespace vigil {

pulse::http::Response DeleteAccountHandler::operator()(
    const pulse::http::Request& request) const {
  auto name = request.path_params.find("name");
  if (name == request.path_params.end()) {
    pulse::Log() << "DeleteAccount: missing parameter 'name'";
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Log() << "DeleteAccount: handling request (name='" << name->second
               << "')";

  if (pulse::Result<void> account = dao_.DeleteAccount(name->second);
      !account.ok()) {
    pulse::Log() << "DeleteAccount: delete failed: " << account.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "DeleteAccount: delete succeeded (name='" << name->second
               << "')";

  return pulse::http::Response{.content_type = "", .status = 204, .body = ""};
}

}  // namespace vigil
