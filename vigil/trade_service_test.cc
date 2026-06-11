#include "vigil/trade_service.h"

#include <optional>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/core/error.h"
#include "pulse/core/result.h"
#include "pulse/core/result_or_die.h"
#include "vigil/db/account.h"
#include "vigil/db/accounts_dao.h"
#include "vigil/db/database.h"
#include "vigil/db/holding.h"
#include "vigil/db/holdings_dao.h"
#include "vigil/db/trade.h"
#include "vigil/db/trades_dao.h"

namespace vigil {

namespace {

using ::testing::Eq;

class TradeServiceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_ = pulse::unwrap_or_die(Database::Open(":memory:"));
    pulse::die_if_error(db_.Initialize());

    accounts_dao_.emplace(db_);
    pulse::die_if_error(
        accounts_dao_->CreateAccount("brokerage", Account::Type::kBrokerage));

    holdings_dao_.emplace(db_);
    trades_dao_.emplace(db_);
    service_.emplace(&db_, &*trades_dao_, &*holdings_dao_);
  }

  Database db_;
  std::optional<AccountsDao> accounts_dao_;
  std::optional<HoldingsDao> holdings_dao_;
  std::optional<TradesDao> trades_dao_;
  std::optional<TradeService> service_;
};

TEST_F(TradeServiceTest, BuyCreatesHolding) {
  ASSERT_TRUE(service_
                  ->RecordTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kBuy,
                                /*ticker=*/"GOOG",
                                /*shares=*/10.0,
                                /*price=*/150.0,
                                /*description=*/std::nullopt)
                  .ok());

  pulse::Result<Holding> holding =
      holdings_dao_->GetHolding(/*account_name=*/"brokerage",
                                /*ticker=*/"GOOG");
  ASSERT_TRUE(holding.ok());
  EXPECT_THAT(holding->shares, Eq(10.0));
}

TEST_F(TradeServiceTest, BuyAddsToExistingHolding) {
  ASSERT_TRUE(service_
                  ->RecordTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kBuy,
                                /*ticker=*/"GOOG",
                                /*shares=*/10.0,
                                /*price=*/150.0,
                                /*description=*/std::nullopt)
                  .ok());
  ASSERT_TRUE(service_
                  ->RecordTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kBuy,
                                /*ticker=*/"GOOG",
                                /*shares=*/5.0,
                                /*price=*/155.0,
                                /*description=*/std::nullopt)
                  .ok());

  pulse::Result<Holding> holding =
      holdings_dao_->GetHolding(/*account_name=*/"brokerage",
                                /*ticker=*/"GOOG");
  ASSERT_TRUE(holding.ok());
  EXPECT_THAT(holding->shares, Eq(15.0));
}

TEST_F(TradeServiceTest, BuyCreatesTrade) {
  ASSERT_TRUE(service_
                  ->RecordTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kBuy,
                                /*ticker=*/"GOOG",
                                /*shares=*/10.0,
                                /*price=*/150.0,
                                /*description=*/std::nullopt)
                  .ok());

  pulse::Result<std::vector<Trade>> trades =
      trades_dao_->ListTrades(/*account_name=*/"brokerage");
  ASSERT_TRUE(trades.ok());
  ASSERT_THAT(trades->size(), Eq(1));
  EXPECT_THAT((*trades)[0].type, Eq(Trade::Type::kBuy));
  EXPECT_THAT((*trades)[0].ticker, Eq("GOOG"));
  EXPECT_THAT((*trades)[0].shares, Eq(10.0));
  EXPECT_THAT((*trades)[0].price, Eq(150.0));
}

TEST_F(TradeServiceTest, SellReducesHolding) {
  ASSERT_TRUE(service_
                  ->RecordTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kBuy,
                                /*ticker=*/"GOOG",
                                /*shares=*/10.0,
                                /*price=*/150.0,
                                /*description=*/std::nullopt)
                  .ok());
  ASSERT_TRUE(service_
                  ->RecordTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kSell,
                                /*ticker=*/"GOOG",
                                /*shares=*/3.0,
                                /*price=*/160.0,
                                /*description=*/std::nullopt)
                  .ok());

  pulse::Result<Holding> holding =
      holdings_dao_->GetHolding(/*account_name=*/"brokerage",
                                /*ticker=*/"GOOG");
  ASSERT_TRUE(holding.ok());
  EXPECT_THAT(holding->shares, Eq(7.0));
}

TEST_F(TradeServiceTest, SellToZeroDeletesHolding) {
  ASSERT_TRUE(service_
                  ->RecordTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kBuy,
                                /*ticker=*/"GOOG",
                                /*shares=*/10.0,
                                /*price=*/150.0,
                                /*description=*/std::nullopt)
                  .ok());
  ASSERT_TRUE(service_
                  ->RecordTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kSell,
                                /*ticker=*/"GOOG",
                                /*shares=*/10.0,
                                /*price=*/160.0,
                                /*description=*/std::nullopt)
                  .ok());

  pulse::Result<Holding> holding =
      holdings_dao_->GetHolding(/*account_name=*/"brokerage",
                                /*ticker=*/"GOOG");
  EXPECT_FALSE(holding.ok());
  EXPECT_THAT(holding.error().code, Eq(pulse::Error::Code::kNotFound));
}

TEST_F(TradeServiceTest, SellWithNoPositionFails) {
  pulse::Result<void> result =
      service_->RecordTrade(/*account_name=*/"brokerage",
                            /*type=*/Trade::Type::kSell,
                            /*ticker=*/"GOOG",
                            /*shares=*/10.0,
                            /*price=*/150.0,
                            /*description=*/std::nullopt);
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.error().code, Eq(pulse::Error::Code::kFailedPrecondition));
}

TEST_F(TradeServiceTest, SellInsufficientSharesFails) {
  ASSERT_TRUE(service_
                  ->RecordTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kBuy,
                                /*ticker=*/"GOOG",
                                /*shares=*/10.0,
                                /*price=*/150.0,
                                /*description=*/std::nullopt)
                  .ok());

  pulse::Result<void> result =
      service_->RecordTrade(/*account_name=*/"brokerage",
                            /*type=*/Trade::Type::kSell,
                            /*ticker=*/"GOOG",
                            /*shares=*/15.0,
                            /*price=*/160.0,
                            /*description=*/std::nullopt);
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.error().code, Eq(pulse::Error::Code::kFailedPrecondition));
}

TEST_F(TradeServiceTest, FailureRollsBackTrade) {
  pulse::Result<void> result =
      service_->RecordTrade(/*account_name=*/"brokerage",
                            /*type=*/Trade::Type::kSell,
                            /*ticker=*/"GOOG",
                            /*shares=*/10.0,
                            /*price=*/150.0,
                            /*description=*/std::nullopt);
  EXPECT_FALSE(result.ok());

  pulse::Result<std::vector<Trade>> trades =
      trades_dao_->ListTrades(/*account_name=*/"brokerage");
  ASSERT_TRUE(trades.ok());
  EXPECT_THAT(trades->size(), Eq(0));
}

TEST_F(TradeServiceTest, NonexistentAccountFails) {
  pulse::Result<void> result =
      service_->RecordTrade(/*account_name=*/"nonexistent",
                            /*type=*/Trade::Type::kBuy,
                            /*ticker=*/"GOOG",
                            /*shares=*/10.0,
                            /*price=*/150.0,
                            /*description=*/std::nullopt);
  EXPECT_FALSE(result.ok());
}

TEST_F(TradeServiceTest, MultipleTickers) {
  ASSERT_TRUE(service_
                  ->RecordTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kBuy,
                                /*ticker=*/"GOOG",
                                /*shares=*/10.0,
                                /*price=*/150.0,
                                /*description=*/std::nullopt)
                  .ok());
  ASSERT_TRUE(service_
                  ->RecordTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kBuy,
                                /*ticker=*/"AAPL",
                                /*shares=*/5.0,
                                /*price=*/200.0,
                                /*description=*/std::nullopt)
                  .ok());

  EXPECT_THAT(holdings_dao_
                  ->GetHolding(/*account_name=*/"brokerage",
                               /*ticker=*/"GOOG")
                  ->shares,
              Eq(10.0));
  EXPECT_THAT(holdings_dao_
                  ->GetHolding(/*account_name=*/"brokerage",
                               /*ticker=*/"AAPL")
                  ->shares,
              Eq(5.0));
}

}  // namespace

}  // namespace vigil
