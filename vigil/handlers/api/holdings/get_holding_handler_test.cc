#include "vigil/handlers/api/holdings/get_holding_handler.h"

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
#include "vigil/db/holdings_dao.h"
#include "vigil/db/trade.h"
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
using ::testing::HasSubstr;

class GetHoldingHandlerTest : public ::testing::Test {
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

    ServerContext<HoldingsDao*> ctx;
    ctx.set(holdings_dao_.get());
    router_ = pulse::UnwrapOrDie(Router::Make<Routes<GetHoldingHandler>>(ctx));
  }

  Response RunMethod(Request req) {
    return (
        *router_.Match(GetHoldingHandler::kMethod, GetHoldingHandler::kPath)
             ->handler)(std::move(req));
  }

  Database db_;
  std::unique_ptr<AccountsDao> accounts_dao_;
  std::unique_ptr<HoldingsDao> holdings_dao_;
  std::unique_ptr<TradesDao> trades_dao_;
  std::unique_ptr<TradeService> service_;
  Router router_;
};

TEST_F(GetHoldingHandlerTest, MissingNameParam) {
  EXPECT_THAT(RunMethod(Request{}).status, Eq(400));
}

TEST_F(GetHoldingHandlerTest, MissingTickerParam) {
  EXPECT_THAT(RunMethod(Request{.path = {{"name", "brokerage"}}}).status,
              Eq(400));
}

TEST_F(GetHoldingHandlerTest, NotFound) {
  EXPECT_THAT(
      RunMethod(Request{.path = {{"name", "brokerage"}, {"ticker", "GOOG"}}})
          .status,
      Eq(404));
}

TEST_F(GetHoldingHandlerTest, GetHolding) {
  pulse::DieIfError(service_->RecordTrade(
      /*account_name=*/"brokerage", /*type=*/Trade::Type::kBuy,
      /*ticker=*/"GOOG", /*shares=*/10.0, /*price=*/150.0,
      /*description=*/std::nullopt));

  Response response =
      RunMethod(Request{.path = {{"name", "brokerage"}, {"ticker", "GOOG"}}});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.body, HasSubstr(R"("ticker":"GOOG")"));
  EXPECT_THAT(response.body, HasSubstr(R"("account_name":"brokerage")"));
}

}  // namespace

}  // namespace vigil
