#include "vigil/db/merchant_aliases_dao.h"

#include <optional>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/core/error.h"
#include "pulse/core/result.h"
#include "vigil/db/database.h"
#include "vigil/db/merchant_alias.h"

namespace vigil {

namespace {

using ::testing::Eq;

class MerchantAliasesDaoTest : public ::testing::Test {
 protected:
  void SetUp() override {
    pulse::Result<Database> db = Database::Open(":memory:");
    ASSERT_TRUE(db.ok()) << db.error().message;

    db_.emplace(*std::move(db));

    pulse::Result<void> err = db_->Initialize();
    ASSERT_TRUE(err.ok()) << err.error().message;

    dao_.emplace(*db_);
  }

  std::optional<Database> db_;
  std::optional<MerchantAliasesDao> dao_;
};

TEST_F(MerchantAliasesDaoTest, CreateAndGet) {
  ASSERT_TRUE(dao_->CreateAlias("RAW_MERCHANT_1", "Merchant One").ok());
  pulse::Result<MerchantAlias> result = dao_->GetAlias("RAW_MERCHANT_1");
  ASSERT_TRUE(result.ok()) << result.error().message;
  EXPECT_THAT(*result, Eq(MerchantAlias{.merchant = "RAW_MERCHANT_1",
                                        .display_name = "Merchant One"}));
}

TEST_F(MerchantAliasesDaoTest, NotFound) {
  pulse::Result<MerchantAlias> result = dao_->GetAlias("nonexistent");
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.error().code, Eq(pulse::Error::Code::kNotFound));
}

TEST_F(MerchantAliasesDaoTest, DuplicateCreateFails) {
  ASSERT_TRUE(dao_->CreateAlias("RAW_MERCHANT_1", "Merchant One").ok());
  pulse::Result<void> result =
      dao_->CreateAlias("RAW_MERCHANT_1", "Merchant One");
  EXPECT_FALSE(result.ok());
}

TEST_F(MerchantAliasesDaoTest, DeleteAlias) {
  ASSERT_TRUE(dao_->CreateAlias("RAW_MERCHANT_1", "Merchant One").ok());
  pulse::Result<void> deleted = dao_->DeleteAlias("RAW_MERCHANT_1");
  ASSERT_TRUE(deleted.ok()) << deleted.error().message;
  pulse::Result<MerchantAlias> result = dao_->GetAlias("RAW_MERCHANT_1");
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.error().code, Eq(pulse::Error::Code::kNotFound));
}

TEST_F(MerchantAliasesDaoTest, DeleteThenRecreate) {
  ASSERT_TRUE(dao_->CreateAlias("RAW_MERCHANT_1", "Merchant One").ok());
  ASSERT_TRUE(dao_->DeleteAlias("RAW_MERCHANT_1").ok());
  EXPECT_TRUE(dao_->CreateAlias("RAW_MERCHANT_1", "Merchant One Renamed").ok());
}

TEST_F(MerchantAliasesDaoTest, DeleteAliasLeavesOthers) {
  ASSERT_TRUE(dao_->CreateAlias("RAW_MERCHANT_1", "Merchant One").ok());
  ASSERT_TRUE(dao_->CreateAlias("RAW_MERCHANT_2", "Merchant Two").ok());
  ASSERT_TRUE(dao_->DeleteAlias("RAW_MERCHANT_1").ok());
  EXPECT_FALSE(dao_->GetAlias("RAW_MERCHANT_1").ok());
  EXPECT_TRUE(dao_->GetAlias("RAW_MERCHANT_2").ok());
}

}  // namespace

}  // namespace vigil
