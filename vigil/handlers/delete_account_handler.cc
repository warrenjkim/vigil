#include "vigil/handlers/delete_account_handler.h"

#include <string>

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/accounts_dao.h"

namespace vigil {

pulse::http::Response DeleteAccountHandler::operator()(
    const pulse::http::Request& request) const {
  auto name = request.path.Get<std::string>("name");
  if (!name.ok()) {
    pulse::Log() << "DeleteAccount: getting 'name': " << name.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Log() << "DeleteAccount: handling request (name='" << *name << "')";

  if (pulse::Result<void> account = dao_.DeleteAccount(*name); !account.ok()) {
    pulse::Log() << "DeleteAccount: delete failed: " << account.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "DeleteAccount: delete succeeded (name='" << *name << "')";

  return pulse::http::Response{.content_type = "", .status = 204, .body = ""};
}

}  // namespace vigil
