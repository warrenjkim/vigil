#include "vigil/handlers/list_transactions_handler.h"

#include <memory>
#include <optional>
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
      "checking", Transaction::Type::kDeposit, 100.0, std::nullopt));

  Response response = RunMethod(Request{.path = {{"name", "checking"}}});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("type":"DEPOSIT")"));
  EXPECT_THAT(response.body, HasSubstr(R"("amount":100)"));
  EXPECT_THAT(response.body, HasSubstr(R"("account_name":"checking")"));
}

TEST_F(ListTransactionsHandlerTest, NullDescription) {
  pulse::DieIfError(transactions_dao_->CreateTransaction(
      "checking", Transaction::Type::kDeposit, 100.0, std::nullopt));

  Response response = RunMethod(Request{.path = {{"name", "checking"}}});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("description":null)"));
}

TEST_F(ListTransactionsHandlerTest, WithDescription) {
  pulse::DieIfError(transactions_dao_->CreateTransaction(
      "checking", Transaction::Type::kDeposit, 100.0, "paycheck"));

  Response response = RunMethod(Request{.path = {{"name", "checking"}}});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("description":"paycheck")"));
}

TEST_F(ListTransactionsHandlerTest, MultipleTransactions) {
  pulse::DieIfError(transactions_dao_->CreateTransaction(
      "checking", Transaction::Type::kDeposit, 100.0, std::nullopt));
  pulse::DieIfError(transactions_dao_->CreateTransaction(
      "checking", Transaction::Type::kWithdrawal, 50.0, std::nullopt));

  Response response = RunMethod(Request{.path = {{"name", "checking"}}});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("type":"DEPOSIT")"));
  EXPECT_THAT(response.body, HasSubstr(R"("type":"WITHDRAWAL")"));
}

TEST_F(ListTransactionsHandlerTest, IsolatedByAccount) {
  pulse::DieIfError(
      accounts_dao_->CreateAccount("savings", Account::Type::kSavings));
  pulse::DieIfError(transactions_dao_->CreateTransaction(
      "savings", Transaction::Type::kDeposit, 999.0, std::nullopt));
  pulse::DieIfError(transactions_dao_->CreateTransaction(
      "checking", Transaction::Type::kDeposit, 100.0, std::nullopt));

  Response response = RunMethod(Request{.path = {{"name", "checking"}}});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("amount":100)"));
}

}  // namespace

}  // namespace vigil
