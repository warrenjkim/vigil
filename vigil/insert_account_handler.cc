#include <iostream>
#include <memory>

#include "pulse/core/result.h"
#include "pulse/http/handler.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/account.h"
#include "vigil/db/accounts_dao.h"

namespace vigil {

namespace {

class InsertAccountHandler final : public pulse::http::Handler {
 public:
  explicit InsertAccountHandler(AccountsDao* dao) : dao(*dao) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override {
    auto name = request.query_params.find("name");
    if (name == request.query_params.end()) {
      std::cerr << "[" << __FILE__ << ":" << __LINE__
                << "] InsertAccount: missing parameter 'name'\n";
      return pulse::http::Response{.content_type = "application/json",
                                   .status = 400,
                                   .body = R"({"status": "invalid argument"})"};
    }

    auto type = request.query_params.find("type");
    if (type == request.query_params.end()) {
      std::cerr << "[" << __FILE__ << ":" << __LINE__
                << "] InsertAccount: missing parameter 'type'\n";
      return pulse::http::Response{.content_type = "application/json",
                                   .status = 400,
                                   .body = R"({"status": "invalid argument"})"};
    }

    Account::Type account_type = to_account_type(type->second);
    if (account_type == Account::Type::kUnknown) {
      std::cerr << "[" << __FILE__ << ":" << __LINE__
                << "] InsertAccount: unrecognized type '" << type->second
                << "'\n";
      return pulse::http::Response{.content_type = "application/json",
                                   .status = 400,
                                   .body = R"({"status": "invalid argument"})"};
    }

    std::cerr << "[" << __FILE__ << ":" << __LINE__
              << "] InsertAccount: handling request (name='" << name->second
              << "', type='" << type->second << "')\n";

    if (pulse::Result<void> account =
            dao.InsertAccount(name->second, account_type);
        !account.ok()) {
      std::cerr << "[" << __FILE__ << ":" << __LINE__
                << "] InsertAccount: insert failed: " << account.error().message
                << "\n";
      return pulse::http::Response{.content_type = "application/json",
                                   .status = 500,
                                   .body = R"({"status": "internal"})"};
    }

    std::cerr << "[" << __FILE__ << ":" << __LINE__
              << "] InsertAccount: insert succeeded (name='" << name->second
              << "')\n";

    return pulse::http::Response{
        .content_type = "text/plain", .status = 201, .body = "Created"};
  }

 private:
  AccountsDao& dao;
};

}  // namespace

std::unique_ptr<pulse::http::Handler> MakeInsertAccountHandler(
    AccountsDao* dao) {
  return std::make_unique<InsertAccountHandler>(dao);
}

}  // namespace vigil
