#include "vigil/handlers/api/accounts/get_account_handler.h"

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
#include "vigil/db/time.h"
#include "vigil/db/transaction.h"
#include "vigil/db/transactions_dao.h"

namespace vigil {

namespace {

using ::pulse::http::Request;
using ::pulse::http::Response;
using ::pulse::http::Router;
using ::pulse::http::Routes;
using ::pulse::http::ServerContext;
using ::testing::Eq;
using ::testing::HasSubstr;

class GetAccountHandlerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_ = pulse::UnwrapOrDie(Database::Open(":memory:"));
    pulse::DieIfError(db_.Initialize());

    accounts_dao_ = std::make_unique<AccountsDao>(db_);
    transactions_dao_ = std::make_unique<TransactionsDao>(db_);

    ServerContext<AccountsDao*, TransactionsDao*> ctx;
    ctx.set(accounts_dao_.get());
    ctx.set(transactions_dao_.get());
    router_ = pulse::UnwrapOrDie(Router::Make<Routes<GetAccountHandler>>(ctx));
  }

  Response RunMethod(Request req) {
    return (
        *router_.Match(GetAccountHandler::kMethod, GetAccountHandler::kPath)
             ->handler)(std::move(req));
  }

  Database db_;
  std::unique_ptr<AccountsDao> accounts_dao_;
  std::unique_ptr<TransactionsDao> transactions_dao_;
  Router router_;
};

TEST_F(GetAccountHandlerTest, NotFound) {
  EXPECT_THAT(RunMethod(Request{.path = {{"name", "nonexistent"}}}).status,
              Eq(404));
}

TEST_F(GetAccountHandlerTest, GetAccount) {
  pulse::DieIfError(
      accounts_dao_->CreateAccount("checking", Account::Type::kChecking));

  Response response = RunMethod(Request{.path = {{"name", "checking"}}});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("name":"checking")"));
  EXPECT_THAT(response.body, HasSubstr(R"("type":"CHECKING")"));
  EXPECT_THAT(response.body, HasSubstr(R"("balance":0)"));
}

TEST_F(GetAccountHandlerTest, GetAccountBrokerageHasNoBalance) {
  pulse::DieIfError(
      accounts_dao_->CreateAccount("brokerage", Account::Type::kBrokerage));

  Response response = RunMethod(Request{.path = {{"name", "brokerage"}}});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("type":"BROKERAGE")"));
  // balance field should not be present for brokerage accounts
  EXPECT_THAT(response.body, Not(HasSubstr(R"("balance")")));
}

TEST_F(GetAccountHandlerTest, GetAccountWithBalance) {
  pulse::DieIfError(
      accounts_dao_->CreateAccount("checking", Account::Type::kChecking));
  pulse::DieIfError(transactions_dao_->CreateTransaction(
      /*account_name=*/"checking", /*external_id=*/"ext_001",
      Transaction::Type::kDeposit,
      /*amount=*/1000, /*merchant=*/"",
      /*transaction_timestamp=*/Time::FromUnixSeconds(0)));
  pulse::DieIfError(transactions_dao_->CreateTransaction(
      /*account_name=*/"checking", /*external_id=*/"ext_002",
      Transaction::Type::kWithdrawal, /*amount=*/250, /*merchant=*/"",
      /*transaction_timestamp=*/Time::FromUnixSeconds(0)));

  Response response = RunMethod(Request{.path = {{"name", "checking"}}});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("balance":750)"));
}

}  // namespace

}  // namespace vigil
