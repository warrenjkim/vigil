#include "vigil/handlers/get_account_handler.h"

#include <memory>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/core/result_or_die.h"
#include "pulse/http/handler.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/http/router.h"
#include "vigil/db/account.h"
#include "vigil/db/accounts_dao.h"
#include "vigil/db/database.h"

namespace vigil {

namespace {

using ::pulse::http::Request;
using ::pulse::http::Response;
using ::pulse::http::Router;
using ::pulse::http::Routes;
using ::pulse::http::ServerContext;
using ::testing::Eq;

class GetAccountHandlerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_ = pulse::unwrap_or_die(Database::Open(":memory:"));
    pulse::die_if_error(db_.Initialize());
    dao_ = std::make_unique<AccountsDao>(db_);

    ServerContext<AccountsDao*> ctx;
    ctx.set(dao_.get());
    router_ =
        pulse::unwrap_or_die(Router::Make<Routes<GetAccountHandler>>(ctx));
  }

  Response RunMethod(Request req) {
    return (
        *router_.match(GetAccountHandler::kMethod, GetAccountHandler::kPath)
             ->handler)(std::move(req));
  }

  Database db_;
  std::unique_ptr<AccountsDao> dao_;
  Router router_;
};

TEST_F(GetAccountHandlerTest, NotFound) {
  EXPECT_THAT(
      RunMethod(Request{.path_params = {{"name", "nonexistent"}}}).status,
      Eq(404));
}

TEST_F(GetAccountHandlerTest, GetAccount) {
  pulse::die_if_error(
      dao_->CreateAccount("checking", Account::Type::kChecking));
  EXPECT_THAT(
      RunMethod(Request{.path_params = {{"name", "checking"}}}),
      Eq(Response{
          .content_type = "application/json",
          .status = 200,
          .body = R"({"id": 1, "name": "checking", "type": "CHECKING"})"}));
}

}  // namespace

}  // namespace vigil
