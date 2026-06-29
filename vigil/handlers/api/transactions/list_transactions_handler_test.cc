#include "vigil/handlers/api/transactions/list_transactions_handler.h"

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

class ListTransactionsHandlerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_ = pulse::UnwrapOrDie(Database::Open(":memory:"));
    pulse::DieIfError(db_.Initialize());

    accounts_dao_ = std::make_unique<AccountsDao>(db_);
    pulse::DieIfError(
        accounts_dao_->CreateAccount("checking", Account::Type::kChecking));

    transactions_dao_ = std::make_unique<TransactionsDao>(db_);

    ServerContext<TransactionsDao*> ctx;
    ctx.set(transactions_dao_.get());
    router_ =
        pulse::UnwrapOrDie(Router::Make<Routes<ListTransactionsHandler>>(ctx));
  }

  Response RunMethod(Request req) {
    return (*router_
                 .Match(ListTransactionsHandler::kMethod,
                        ListTransactionsHandler::kPath)
                 ->handler)(std::move(req));
  }

  Database db_;
  std::unique_ptr<AccountsDao> accounts_dao_;
  std::unique_ptr<TransactionsDao> transactions_dao_;
  Router router_;
};

TEST_F(ListTransactionsHandlerTest, MissingNameParam) {
  EXPECT_THAT(RunMethod(Request{}).status, Eq(400));
}

TEST_F(ListTransactionsHandlerTest, EmptyList) {
  Response response = RunMethod(Request{.path = {{"name", "checking"}}});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, Eq("[]"));
}

TEST_F(ListTransactionsHandlerTest, ListsTransactions) {
  pulse::DieIfError(transactions_dao_->CreateTransaction(
      /*account_name=*/"checking", /*external_id=*/"ext_001",
      Transaction::Type::kDeposit, /*amount=*/100, /*merchant=*/"",
      /*transaction_timestamp=*/Time::FromUnixSeconds(0)));

  Response response = RunMethod(Request{.path = {{"name", "checking"}}});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("type":"DEPOSIT")"));
  EXPECT_THAT(response.body, HasSubstr(R"("amount":100)"));
  EXPECT_THAT(response.body, HasSubstr(R"("account_name":"checking")"));
}

TEST_F(ListTransactionsHandlerTest, WithDescription) {
  pulse::DieIfError(transactions_dao_->CreateTransaction(
      /*account_name=*/"checking", /*external_id=*/"ext_001",
      Transaction::Type::kDeposit, /*amount=*/100, /*merchant=*/"paycheck",
      /*transaction_timestamp=*/Time::FromUnixSeconds(0)));

  Response response = RunMethod(Request{.path = {{"name", "checking"}}});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("merchant":"paycheck")"));
}

TEST_F(ListTransactionsHandlerTest, MultipleTransactions) {
  pulse::DieIfError(transactions_dao_->CreateTransaction(
      /*account_name=*/"checking", /*external_id=*/"ext_001",
      Transaction::Type::kDeposit, /*amount=*/100, /*merchant=*/"",
      /*transaction_timestamp=*/Time::FromUnixSeconds(0)));
  pulse::DieIfError(transactions_dao_->CreateTransaction(
      /*account_name=*/"checking", /*external_id=*/"ext_002",
      Transaction::Type::kWithdrawal, /*amount=*/50,
      /*merchant=*/"", /*transaction_timestamp=*/Time::FromUnixSeconds(0)));

  Response response = RunMethod(Request{.path = {{"name", "checking"}}});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("type":"DEPOSIT")"));
  EXPECT_THAT(response.body, HasSubstr(R"("type":"WITHDRAWAL")"));
}

TEST_F(ListTransactionsHandlerTest, IsolatedByAccount) {
  pulse::DieIfError(
      accounts_dao_->CreateAccount("savings", Account::Type::kSavings));
  pulse::DieIfError(transactions_dao_->CreateTransaction(
      /*account_name=*/"savings", /*external_id=*/"ext_001",
      Transaction::Type::kDeposit, /*amount=*/999, /*merchant=*/"",
      /*transaction_timestamp=*/Time::FromUnixSeconds(0)));
  pulse::DieIfError(transactions_dao_->CreateTransaction(
      /*account_name=*/"checking", /*external_id=*/"ext_002",
      Transaction::Type::kDeposit, /*amount=*/100, /*merchant=*/"",
      /*transaction_timestamp=*/Time::FromUnixSeconds(0)));

  Response response = RunMethod(Request{.path = {{"name", "checking"}}});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("amount":100)"));
}

}  // namespace

}  // namespace vigil
