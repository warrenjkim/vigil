#include "vigil/db/holdings_dao.h"

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
#include "vigil/db/holding.h"

namespace vigil {

namespace {

using ::testing::Eq;
using ::testing::IsEmpty;
using ::testing::SizeIs;

class HoldingsDaoTest : public ::testing::Test {
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
  std::optional<HoldingsDao> dao_;
};

TEST_F(HoldingsDaoTest, CreateAndGet) {
  ASSERT_TRUE(dao_->CreateHolding("brokerage", "GOOG", 10.0).ok());

  pulse::Result<Holding> result = dao_->GetHolding("brokerage", "GOOG");
  ASSERT_TRUE(result.ok()) << result.error().message;
  EXPECT_THAT(result->account_name, Eq("brokerage"));
  EXPECT_THAT(result->ticker, Eq("GOOG"));
  EXPECT_THAT(result->shares, Eq(10.0));
}

TEST_F(HoldingsDaoTest, NotFound) {
  pulse::Result<Holding> result = dao_->GetHolding("brokerage", "GOOG");
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.error().code, Eq(pulse::Error::Code::kNotFound));
}

TEST_F(HoldingsDaoTest, DuplicateCreateFails) {
  ASSERT_TRUE(dao_->CreateHolding("brokerage", "GOOG", 10.0).ok());
  EXPECT_FALSE(dao_->CreateHolding("brokerage", "GOOG", 5.0).ok());
}

TEST_F(HoldingsDaoTest, CreateForNonexistentAccountFails) {
  EXPECT_FALSE(dao_->CreateHolding("nonexistent", "GOOG", 10.0).ok());
}

TEST_F(HoldingsDaoTest, ListEmpty) {
  pulse::Result<std::vector<Holding>> result = dao_->ListHoldings("brokerage");
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(*result, IsEmpty());
}

TEST_F(HoldingsDaoTest, ListMultiple) {
  ASSERT_TRUE(dao_->CreateHolding("brokerage", "GOOG", 10.0).ok());
  ASSERT_TRUE(dao_->CreateHolding("brokerage", "AMZN", 5.0).ok());

  pulse::Result<std::vector<Holding>> result = dao_->ListHoldings("brokerage");
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(*result, SizeIs(2));
}

TEST_F(HoldingsDaoTest, ListIsolatedByAccount) {
  ASSERT_TRUE(
      accounts_dao_->CreateAccount("brokerage2", Account::Type::kBrokerage)
          .ok());
  ASSERT_TRUE(dao_->CreateHolding("brokerage", "GOOG", 10.0).ok());
  ASSERT_TRUE(dao_->CreateHolding("brokerage2", "GOOG", 5.0).ok());

  pulse::Result<std::vector<Holding>> result = dao_->ListHoldings("brokerage");
  ASSERT_TRUE(result.ok());
  ASSERT_THAT(*result, SizeIs(1));
  EXPECT_THAT(result->at(0).shares, Eq(10.0));
}

TEST_F(HoldingsDaoTest, UpdateShares) {
  ASSERT_TRUE(dao_->CreateHolding("brokerage", "GOOG", 10.0).ok());
  ASSERT_TRUE(dao_->UpdateShares("brokerage", "GOOG", 20.0).ok());

  pulse::Result<Holding> result = dao_->GetHolding("brokerage", "GOOG");
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(result->shares, Eq(20.0));
}

TEST_F(HoldingsDaoTest, DeleteHolding) {
  ASSERT_TRUE(dao_->CreateHolding("brokerage", "GOOG", 10.0).ok());
  ASSERT_TRUE(dao_->DeleteHolding("brokerage", "GOOG").ok());

  EXPECT_FALSE(dao_->GetHolding("brokerage", "GOOG").ok());
}

TEST_F(HoldingsDaoTest, DeleteNonexistent) {
  EXPECT_TRUE(dao_->DeleteHolding("brokerage", "GOOG").ok());
}

}  // namespace

}  // namespace vigil
