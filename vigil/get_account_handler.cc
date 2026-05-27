#include <iostream>
#include <memory>
#include <string>

#include "pulse/core/result.h"
#include "pulse/core/stringify.h"
#include "pulse/http/handler.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/account.h"
#include "vigil/db/accounts_dao.h"

namespace vigil {

namespace {

class GetAccountHandler final : public pulse::http::Handler {
 public:
  explicit GetAccountHandler(AccountsDao* dao) : dao(*dao) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override {
    auto name = request.path_params.find("name");
    if (name == request.path_params.end()) {
      std::cerr << "[" << __FILE__ << ":" << __LINE__
                << "] GetAccount: missing parameter 'name'\n";
      return pulse::http::Response{.content_type = "application/json",
                                   .status = 400,
                                   .body = R"({"status": "invalid argument"})"};
    }

    std::cerr << "[" << __FILE__ << ":" << __LINE__
              << "] GetAccount: handling request (name='" << name->second
              << "')\n";

    pulse::Result<Account> account = dao.GetAccount(name->second);
    if (!account.ok()) {
      std::cerr << "[" << __FILE__ << ":" << __LINE__
                << "] GetAccount: lookup failed: " << account.error().message
                << "\n";
      return pulse::http::Response{.content_type = "application/json",
                                   .status = 404,
                                   .body = R"({"status": "not found"})"};
    }

    std::cerr << "[" << __FILE__ << ":" << __LINE__
              << "] GetAccount: lookup succeeded: "
              << pulse::to_string(*account) << "\n";

    return pulse::http::Response{
        .content_type = "application/json",
        .status = 200,
        .body = "{\"id\": " + std::to_string(account->id) + ", \"name\": \"" +
                account->name + "\", \"type\": \"" +
                pulse::to_string(account->type) + "\"}"};
  }

 private:
  AccountsDao& dao;
};

}  // namespace

std::unique_ptr<pulse::http::Handler> MakeGetAccountHandler(AccountsDao* dao) {
  return std::make_unique<GetAccountHandler>(dao);
}

}  // namespace vigil
