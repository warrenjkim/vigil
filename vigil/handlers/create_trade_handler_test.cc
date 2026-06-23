#include "vigil/handlers/create_trade_handler.h"

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
#include "vigil/db/holdings_dao.h"
#include "vigil/db/trades_dao.h"
#include "vigil/trade_service.h"

namespace vigil {

namespace {

using ::pulse::http::Request;
using ::pulse::http::Response;
using ::pulse::http::Router;
using ::pulse::http::Routes;
using ::pulse::http::ServerContext;
using ::testing::Eq;

class CreateTradeHandlerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_ = pulse::UnwrapOrDie(Database::Open(":memory:"));
    pulse::DieIfError(db_.Initialize());

    accounts_dao_ = std::make_unique<AccountsDao>(db_);
    pulse::DieIfError(
        accounts_dao_->CreateAccount("brokerage", Account::Type::kBrokerage));

    holdings_dao_ = std::make_unique<HoldingsDao>(db_);
    trades_dao_ = std::make_unique<TradesDao>(db_);
    service_ = std::make_unique<TradeService>(&db_, trades_dao_.get(),
                                              holdings_dao_.get());

    ServerContext<TradeService*> ctx;
    ctx.set(service_.get());
    router_ = pulse::UnwrapOrDie(Router::Make<Routes<CreateTradeHandler>>(ctx));
  }

  Response RunMethod(Request req) {
    return (
        *router_.Match(CreateTradeHandler::kMethod, CreateTradeHandler::kPath)
             ->handler)(std::move(req));
  }

  Database db_;
  std::unique_ptr<AccountsDao> accounts_dao_;
  std::unique_ptr<HoldingsDao> holdings_dao_;
  std::unique_ptr<TradesDao> trades_dao_;
  std::unique_ptr<TradeService> service_;
  Router router_;
};

TEST_F(CreateTradeHandlerTest, CreateTrade) {
  EXPECT_THAT(
      RunMethod(Request{.path = {{"name", "brokerage"}}, .body = R"({
            "type": "BUY",
            "ticker": "GOOG",
            "shares": 10.0,
            "price": 150.0
          })"}),
      Eq(Response{
          .content_type = "text/plain", .status = 201, .body = "Created"}));
}

TEST_F(CreateTradeHandlerTest, MissingNameParam) {
  EXPECT_THAT(RunMethod(Request{.body = R"({
            "type": "BUY",
            "ticker": "GOOG",
            "shares": 10.0,
            "price": 150.0
          })"})
                  .status,
              Eq(400));
}

TEST_F(CreateTradeHandlerTest, MissingType) {
  EXPECT_THAT(RunMethod(Request{.path = {{"name", "brokerage"}}, .body = R"({
            "ticker": "GOOG",
            "shares": 10.0,
            "price": 150.0
          })"})
                  .status,
              Eq(400));
}

TEST_F(CreateTradeHandlerTest, MissingTicker) {
  EXPECT_THAT(RunMethod(Request{.path = {{"name", "brokerage"}}, .body = R"({
            "type": "BUY",
            "shares": 10.0,
            "price": 150.0
          })"})
                  .status,
              Eq(400));
}

TEST_F(CreateTradeHandlerTest, MissingShares) {
  EXPECT_THAT(RunMethod(Request{.path = {{"name", "brokerage"}}, .body = R"({
            "type": "BUY",
            "ticker": "GOOG",
            "price": 150.0
          })"})
                  .status,
              Eq(400));
}

TEST_F(CreateTradeHandlerTest, MissingPrice) {
  EXPECT_THAT(RunMethod(Request{.path = {{"name", "brokerage"}}, .body = R"({
            "type": "BUY",
            "ticker": "GOOG",
            "shares": 10.0
          })"})
                  .status,
              Eq(400));
}

TEST_F(CreateTradeHandlerTest, UnrecognizedType) {
  EXPECT_THAT(RunMethod(Request{.path = {{"name", "brokerage"}}, .body = R"({
            "type": "bad_type",
            "ticker": "GOOG",
            "shares": 10.0,
            "price": 150.0
          })"})
                  .status,
              Eq(400));
}

TEST_F(CreateTradeHandlerTest, SellWithNoPosition) {
  EXPECT_THAT(RunMethod(Request{.path = {{"name", "brokerage"}}, .body = R"({
            "type": "SELL",
            "ticker": "GOOG",
            "shares": 10.0,
            "price": 150.0
          })"})
                  .status,
              Eq(422));
}

TEST_F(CreateTradeHandlerTest, NonexistentAccount) {
  EXPECT_THAT(RunMethod(Request{.path = {{"name", "nonexistent"}}, .body = R"({
            "type": "BUY",
            "ticker": "GOOG",
            "shares": 10.0,
            "price": 150.0
          })"})
                  .status,
              Eq(500));
}

TEST_F(CreateTradeHandlerTest, WithDescription) {
  EXPECT_THAT(RunMethod(Request{.path = {{"name", "brokerage"}}, .body = R"({
            "type": "BUY",
            "ticker": "GOOG",
            "shares": 10.0,
            "price": 150.0,
            "description": "initial position"
          })"})
                  .status,
              Eq(201));
}

}  // namespace

}  // namespace vigil
