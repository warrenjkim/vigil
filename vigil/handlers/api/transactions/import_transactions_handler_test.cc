#include "vigil/handlers/api/transactions/import_transactions_handler.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/core/result.h"
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
#include "vigil/services/transaction_service.h"

namespace vigil {

namespace {

using ::pulse::http::Request;
using ::pulse::http::Response;
using ::pulse::http::Router;
using ::pulse::http::Routes;
using ::pulse::http::ServerContext;
using ::testing::Eq;
using ::testing::HasSubstr;
using ::testing::ValuesIn;

class ImportTransactionsHandlerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_ = pulse::UnwrapOrDie(Database::Open(":memory:"));
    pulse::DieIfError(db_.Initialize());

    accounts_dao_ = std::make_unique<AccountsDao>(db_);
    pulse::DieIfError(
        accounts_dao_->CreateAccount(/*name=*/"checking",
                                     /*type=*/Account::Type::kChecking));

    transactions_dao_ = std::make_unique<TransactionsDao>(db_);
    service_ = std::make_unique<TransactionService>(&db_, accounts_dao_.get(),
                                                    transactions_dao_.get());

    ServerContext<TransactionService*> ctx;
    ctx.set(service_.get());
    router_ = pulse::UnwrapOrDie(
        Router::Make<Routes<ImportTransactionsHandler>>(ctx));
  }

  Response RunMethod(Request req) {
    return (*router_
                 .Match(ImportTransactionsHandler::kMethod,
                        ImportTransactionsHandler::kPath)
                 ->handler)(std::move(req));
  }

  Database db_;
  std::unique_ptr<AccountsDao> accounts_dao_;
  std::unique_ptr<TransactionsDao> transactions_dao_;
  std::unique_ptr<TransactionService> service_;
  Router router_;
};

struct ImportTransactionsHandlerParams {
  std::string test_name;
  Request request;
  int expected_status;
};

class ImportTransactionsHandlerParamTest
    : public ImportTransactionsHandlerTest,
      public ::testing::WithParamInterface<ImportTransactionsHandlerParams> {};

TEST_P(ImportTransactionsHandlerParamTest, ReturnsExpectedStatus) {
  EXPECT_THAT(RunMethod(GetParam().request).status,
              Eq(GetParam().expected_status));
}

INSTANTIATE_TEST_SUITE_P(
    ImportTransactionsHandlerTests, ImportTransactionsHandlerParamTest,
    ValuesIn<ImportTransactionsHandlerParams>(
        {{.test_name = "MissingNameParam",
          .request = Request{.body = R"([{
              "external_id": "ext_001",
              "type": "DEPOSIT",
              "amount": 100.0,
              "merchant": "",
              "transaction_timestamp": 0
            }])"},
          .expected_status = 400},
         {.test_name = "MissingExternalId",
          .request = Request{.path = {{"name", "checking"}}, .body = R"([{
              "type": "DEPOSIT",
              "amount": 100.0,
              "merchant": "",
              "transaction_timestamp": 0
            }])"},
          .expected_status = 400},
         {.test_name = "MissingType",
          .request = Request{.path = {{"name", "checking"}}, .body = R"([{
              "external_id": "ext_001",
              "amount": 100.0,
              "merchant": "",
              "transaction_timestamp": 0
            }])"},
          .expected_status = 400},
         {.test_name = "MissingAmount",
          .request = Request{.path = {{"name", "checking"}}, .body = R"([{
              "external_id": "ext_001",
              "type": "DEPOSIT",
              "merchant": "",
              "transaction_timestamp": 0
            }])"},
          .expected_status = 400},
         {.test_name = "MissingMerchant",
          .request = Request{.path = {{"name", "checking"}}, .body = R"([{
              "external_id": "ext_001",
              "type": "DEPOSIT",
              "amount": 100.0,
              "transaction_timestamp": 0
            }])"},
          .expected_status = 400},
         {.test_name = "MissingTransactionTimestamp",
          .request = Request{.path = {{"name", "checking"}}, .body = R"([{
              "external_id": "ext_001",
              "type": "DEPOSIT",
              "amount": 100.0,
              "merchant": ""
            }])"},
          .expected_status = 400},
         {.test_name = "UnrecognizedType",
          .request = Request{.path = {{"name", "checking"}}, .body = R"([{
              "external_id": "ext_001",
              "type": "bad_type",
              "amount": 100.0,
              "merchant": "",
              "transaction_timestamp": 0
            }])"},
          .expected_status = 400},
         {.test_name = "NonexistentAccount",
          .request = Request{.path = {{"name", "nonexistent"}}, .body = R"([{
              "external_id": "ext_001",
              "type": "DEPOSIT",
              "amount": 100.0,
              "merchant": "",
              "transaction_timestamp": 0
            }])"},
          .expected_status = 404},
         {.test_name = "NotAnArray",
          .request = Request{.path = {{"name", "checking"}},
                             .body = R"({"external_id": "ext_001"})"},
          .expected_status = 400}}),
    [](const auto& info) { return info.param.test_name; });

TEST_F(ImportTransactionsHandlerTest, ImportEmpty) {
  Response response =
      RunMethod(Request{.path = {{"name", "checking"}}, .body = R"([])"});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("imported":0)"));
}

TEST_F(ImportTransactionsHandlerTest, ImportSingle) {
  Response response =
      RunMethod(Request{.path = {{"name", "checking"}}, .body = R"([{
        "external_id": "ext_001",
        "type": "DEPOSIT",
        "amount": 100.0,
        "merchant": "paycheck",
        "transaction_timestamp": 0
      }])"});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("imported":1)"));
}

TEST_F(ImportTransactionsHandlerTest, ImportMultiple) {
  Response response =
      RunMethod(Request{.path = {{"name", "checking"}}, .body = R"([
        {
          "external_id": "ext_001",
          "type": "DEPOSIT",
          "amount": 100.0,
          "merchant": "paycheck",
          "transaction_timestamp": 0
        },
        {
          "external_id": "ext_002",
          "type": "WITHDRAWAL",
          "amount": 50.0,
          "merchant": "grocery store",
          "transaction_timestamp": 0
        }
      ])"});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("imported":2)"));
}

TEST_F(ImportTransactionsHandlerTest, ImportIdempotent) {
  const Request request{.path = {{"name", "checking"}}, .body = R"([{
    "external_id": "ext_001",
    "type": "DEPOSIT",
    "amount": 100.0,
    "merchant": "paycheck",
    "transaction_timestamp": 0
  }])"};

  {
    Response response = RunMethod(request);
    EXPECT_THAT(response.status, Eq(200));
    EXPECT_THAT(response.body, HasSubstr(R"("imported":1)"));
  }

  {
    Response response = RunMethod(request);
    EXPECT_THAT(response.status, Eq(200));
    EXPECT_THAT(response.body, HasSubstr(R"("imported":0)"));
  }

  pulse::Result<std::vector<Transaction>> result =
      transactions_dao_->ListTransactions(/*account_name=*/"checking");
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(result->size(), Eq(1));
}

}  // namespace

}  // namespace vigil
