#include "vigil/handlers/list_accounts_handler.h"

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
using ::testing::HasSubstr;

class ListAccountsHandlerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_ = pulse::UnwrapOrDie(Database::Open(":memory:"));
    pulse::DieIfError(db_.Initialize());
    dao_ = std::make_unique<AccountsDao>(db_);

    ServerContext<AccountsDao*> ctx;
    ctx.set(dao_.get());
    router_ =
        pulse::UnwrapOrDie(Router::Make<Routes<ListAccountsHandler>>(ctx));
  }

  Response RunMethod(Request req) {
    return (
        *router_
             .Match(ListAccountsHandler::kMethod, ListAccountsHandler::kPath)
             ->handler)(std::move(req));
  }

  Database db_;
  std::unique_ptr<AccountsDao> dao_;
  Router router_;
};

TEST_F(ListAccountsHandlerTest, EmptyList) {
  Response response = RunMethod(Request{});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, Eq("[]"));
}

TEST_F(ListAccountsHandlerTest, ListsAccounts) {
  pulse::DieIfError(dao_->CreateAccount("checking", Account::Type::kChecking));

  Response response = RunMethod(Request{});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("name":"checking")"));
  EXPECT_THAT(response.body, HasSubstr(R"("type":"CHECKING")"));
}

TEST_F(ListAccountsHandlerTest, MultipleAccounts) {
  pulse::DieIfError(dao_->CreateAccount("checking", Account::Type::kChecking));
  pulse::DieIfError(dao_->CreateAccount("savings", Account::Type::kSavings));

  Response response = RunMethod(Request{});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("name":"checking")"));
  EXPECT_THAT(response.body, HasSubstr(R"("name":"savings")"));
}

}  // namespace

}  // namespace vigil
