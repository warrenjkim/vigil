#include "vigil/db/trades_dao.h"

#include <optional>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/core/error.h"
#include "pulse/core/result.h"
#include "vigil/db/account.h"
#include "vigil/db/accounts_dao.h"
#include "vigil/db/database.h"
#include "vigil/db/time.h"
#include "vigil/db/trade.h"

namespace vigil {

namespace {

using ::testing::Eq;
using ::testing::IsEmpty;
using ::testing::SizeIs;
using ::testing::StrEq;

class TradesDaoTest : public ::testing::Test {
 protected:
  void SetUp() override {
    pulse::Result<Database> db = Database::Open(":memory:");
    ASSERT_TRUE(db.ok()) << db.error().message;
    db_.emplace(std::move(*db));

    pulse::Result<void> err = db_->Initialize();
    ASSERT_TRUE(err.ok()) << err.error().message;

    accounts_dao_.emplace(*db_);
    ASSERT_TRUE(
        accounts_dao_->CreateAccount("brokerage", Account::Type::kBrokerage)
            .ok());

    dao_.emplace(*db_);
  }

  std::optional<Database> db_;
  std::optional<AccountsDao> accounts_dao_;
  std::optional<TradesDao> dao_;
};

TEST_F(TradesDaoTest, CreateAndList) {
  const Time trade_timestamp = Time::FromUnixSeconds(0);
  ASSERT_TRUE(dao_->CreateTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kBuy,
                                /*ticker=*/"GOOG",
                                /*shares=*/10,
                                /*price=*/150,
                                /*description=*/std::nullopt,
                                /*trade_timestamp=*/trade_timestamp)
                  .ok());
  pulse::Result<std::vector<Trade>> result =
      dao_->ListTrades(/*account_name=*/"brokerage");
  ASSERT_TRUE(result.ok()) << result.error().message;
  ASSERT_THAT(*result, SizeIs(1));
  EXPECT_THAT((*result)[0].account_name, StrEq("brokerage"));
  EXPECT_THAT((*result)[0].type, Eq(Trade::Type::kBuy));
  EXPECT_THAT((*result)[0].ticker, StrEq("GOOG"));
  EXPECT_THAT((*result)[0].shares, Eq(10));
  EXPECT_THAT((*result)[0].price, Eq(150));
  EXPECT_FALSE((*result)[0].description.has_value());
  EXPECT_THAT((*result)[0].trade_timestamp, Eq(trade_timestamp));
}

TEST_F(TradesDaoTest, CreateWithDescription) {
  ASSERT_TRUE(dao_->CreateTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kBuy,
                                /*ticker=*/"GOOG",
                                /*shares=*/10,
                                /*price=*/150,
                                /*description=*/"initial position",
                                /*trade_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());
  pulse::Result<std::vector<Trade>> result =
      dao_->ListTrades(/*account_name=*/"brokerage");
  ASSERT_TRUE(result.ok());
  ASSERT_THAT(*result, SizeIs(1));
  ASSERT_TRUE((*result)[0].description.has_value());
  EXPECT_THAT(*(*result)[0].description, StrEq("initial position"));
}

TEST_F(TradesDaoTest, ListEmpty) {
  pulse::Result<std::vector<Trade>> result =
      dao_->ListTrades(/*account_name=*/"brokerage");
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(*result, IsEmpty());
}

TEST_F(TradesDaoTest, ListMultiple) {
  ASSERT_TRUE(dao_->CreateTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kBuy,
                                /*ticker=*/"GOOG",
                                /*shares=*/10,
                                /*price=*/150,
                                /*description=*/std::nullopt,
                                /*trade_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());
  ASSERT_TRUE(dao_->CreateTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kSell,
                                /*ticker=*/"GOOG",
                                /*shares=*/5,
                                /*price=*/160,
                                /*description=*/std::nullopt,
                                /*trade_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());
  pulse::Result<std::vector<Trade>> result =
      dao_->ListTrades(/*account_name=*/"brokerage");
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(*result, SizeIs(2));
}

TEST_F(TradesDaoTest, ListIsolatedByAccount) {
  ASSERT_TRUE(accounts_dao_
                  ->CreateAccount(/*name=*/"brokerage2",
                                  /*type=*/Account::Type::kBrokerage)
                  .ok());
  ASSERT_TRUE(dao_->CreateTrade(/*account_name=*/"brokerage",
                                /*type=*/Trade::Type::kBuy,
                                /*ticker=*/"GOOG",
                                /*shares=*/10,
                                /*price=*/150,
                                /*description=*/std::nullopt,
                                /*trade_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());
  ASSERT_TRUE(dao_->CreateTrade(/*account_name=*/"brokerage2",
                                /*type=*/Trade::Type::kBuy,
                                /*ticker=*/"GOOG",
                                /*shares=*/5,
                                /*price=*/150,
                                /*description=*/std::nullopt,
                                /*trade_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());
  pulse::Result<std::vector<Trade>> result =
      dao_->ListTrades(/*account_name=*/"brokerage");
  ASSERT_TRUE(result.ok());
  ASSERT_THAT(*result, SizeIs(1));
  EXPECT_THAT((*result)[0].shares, Eq(10));
}

TEST_F(TradesDaoTest, CreateForNonexistentAccount) {
  EXPECT_FALSE(dao_->CreateTrade(/*account_name=*/"nonexistent",
                                 /*type=*/Trade::Type::kBuy,
                                 /*ticker=*/"GOOG",
                                 /*shares=*/10,
                                 /*price=*/150,
                                 /*description=*/std::nullopt,
                                 /*trade_timestamp=*/Time::FromUnixSeconds(0))
                   .ok());
}

}  // namespace

}  // namespace vigil
