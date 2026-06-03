#include "vigil/handlers/create_transaction_handler.h"

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/core/result.h"
#include "pulse/core/result_or_die.h"
#include "pulse/core/stringify.h"
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
using ::testing::SizeIs;

class CreateTransactionHandlerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_ = pulse::unwrap_or_die(Database::Open(":memory:"));
    pulse::die_if_error(db_.Initialize());

    accounts_dao_ = std::make_unique<AccountsDao>(db_);
    pulse::die_if_error(
        accounts_dao_->CreateAccount("checking", Account::Type::kChecking));

    transactions_dao_ = std::make_unique<TransactionsDao>(db_);

    ServerContext<TransactionsDao*> ctx;
    ctx.set(transactions_dao_.get());
    router_ = pulse::unwrap_or_die(
        Router::Make<Routes<CreateTransactionHandler>>(ctx));
  }

  Response RunMethod(Request req) {
    return (*router_
                 .match(CreateTransactionHandler::kMethod,
                        CreateTransactionHandler::kPath)
                 ->handler)(std::move(req));
  }

  Database db_;
  std::unique_ptr<AccountsDao> accounts_dao_;
  std::unique_ptr<TransactionsDao> transactions_dao_;
  Router router_;
};

TEST_F(CreateTransactionHandlerTest, CreateTransaction) {
  EXPECT_THAT(
      RunMethod(Request{
          .path = {{"name", "checking"}},
          .query = {{"type", pulse::to_string(Transaction::Type::kDeposit)},
                    {"amount", "100.00"}}}),
      Eq(Response{
          .content_type = "text/plain", .status = 201, .body = "Created"}));
}

TEST_F(CreateTransactionHandlerTest, PersistsTransaction) {
  ASSERT_THAT(
      RunMethod(Request{.path = {{"name", "checking"}},
                        .query = {{"type", pulse::to_string(
                                               Transaction::Type::kDeposit)},
                                  {"amount", "100.00"}}})
          .status,
      Eq(201));

  pulse::Result<std::vector<Transaction>> transactions =
      transactions_dao_->ListTransactions("checking");
  ASSERT_TRUE(transactions.ok());
  ASSERT_THAT(*transactions, SizeIs(1));
  EXPECT_THAT((*transactions)[0].account_name, Eq("checking"));
  EXPECT_THAT((*transactions)[0].type, Eq(Transaction::Type::kDeposit));
  EXPECT_THAT((*transactions)[0].amount, Eq(100.00));
  EXPECT_THAT((*transactions)[0].description, Eq(std::nullopt));
}

TEST_F(CreateTransactionHandlerTest, WithDescription) {
  EXPECT_THAT(
      RunMethod(Request{.path = {{"name", "checking"}},
                        .query = {{"type", pulse::to_string(
                                               Transaction::Type::kDeposit)},
                                  {"amount", "100.00"},
                                  {"description", "paycheck"}}})
          .status,
      Eq(201));
}

TEST_F(CreateTransactionHandlerTest, MissingAccountName) {
  EXPECT_THAT(
      RunMethod(Request{.query = {{"type", pulse::to_string(
                                               Transaction::Type::kDeposit)},
                                  {"amount", "100.00"}}})
          .status,
      Eq(400));
}

TEST_F(CreateTransactionHandlerTest, MissingType) {
  EXPECT_THAT(RunMethod(Request{.path = {{"name", "checking"}},
                                .query = {{"amount", "100.00"}}})
                  .status,
              Eq(400));
}

TEST_F(CreateTransactionHandlerTest, MissingAmount) {
  EXPECT_THAT(
      RunMethod(Request{.path = {{"name", "checking"}},
                        .query = {{"type", pulse::to_string(
                                               Transaction::Type::kDeposit)}}})
          .status,
      Eq(400));
}

TEST_F(CreateTransactionHandlerTest, UnrecognizedType) {
  EXPECT_THAT(
      RunMethod(Request{.path = {{"name", "checking"}},
                        .query = {{"type", "bad_type"}, {"amount", "100.00"}}})
          .status,
      Eq(400));
}

TEST_F(CreateTransactionHandlerTest, InvalidAmount) {
  EXPECT_THAT(
      RunMethod(Request{.path = {{"name", "checking"}},
                        .query = {{"type", pulse::to_string(
                                               Transaction::Type::kDeposit)},
                                  {"amount", "not_a_number"}}})
          .status,
      Eq(400));
}

TEST_F(CreateTransactionHandlerTest, NonexistentAccount) {
  EXPECT_THAT(
      RunMethod(Request{.path = {{"name", "nonexistent"}},
                        .query = {{"type", pulse::to_string(
                                               Transaction::Type::kDeposit)},
                                  {"amount", "100.00"}}})
          .status,
      Eq(500));
}

}  // namespace

}  // namespace vigil
