#include "vigil/db/accounts_dao.h"

#include <optional>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/core/error.h"
#include "pulse/core/result.h"
#include "vigil/db/account.h"
#include "vigil/db/database.h"

namespace vigil {

namespace {

using ::testing::Eq;

class AccountDaoTest : public ::testing::Test {
 protected:
  void SetUp() override {
    pulse::Result<Database> db = Database::Open(":memory:");
    ASSERT_TRUE(db.ok()) << db.error().message;
    db_.emplace(std::move(*db));

    pulse::Result<void> err = db_->Initialize();
    ASSERT_TRUE(err.ok()) << err.error().message;
    dao_.emplace(*db_);
  }

  std::optional<Database> db_;
  std::optional<AccountsDao> dao_;
};

TEST_F(AccountDaoTest, CreateAndGet) {
  ASSERT_TRUE(
      dao_->CreateAccount("Chase Checking", Account::Type::kChecking).ok());

  auto result = dao_->GetAccount("Chase Checking");
  ASSERT_TRUE(result.ok()) << result.error().message;
  EXPECT_THAT(*result, Eq(Account{.id = 1,
                                  .name = "Chase Checking",
                                  .type = Account::Type::kChecking}));
}

TEST_F(AccountDaoTest, NotFound) {
  auto result = dao_->GetAccount("nonexistent");
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.error().code, Eq(pulse::Error::Code::kNotFound));
}

TEST_F(AccountDaoTest, DuplicateCreateFails) {
  ASSERT_TRUE(
      dao_->CreateAccount("Chase Checking", Account::Type::kChecking).ok());
  auto result = dao_->CreateAccount("Chase Checking", Account::Type::kChecking);
  EXPECT_FALSE(result.ok());
}

TEST_F(AccountDaoTest, DeleteAccount) {
  ASSERT_TRUE(
      dao_->CreateAccount("Chase Checking", Account::Type::kChecking).ok());

  pulse::Result<void> deleted = dao_->DeleteAccount("Chase Checking");
  ASSERT_TRUE(deleted.ok()) << deleted.error().message;

  auto result = dao_->GetAccount("Chase Checking");
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.error().code, Eq(pulse::Error::Code::kNotFound));
}

TEST_F(AccountDaoTest, DeleteThenRecreate) {
  ASSERT_TRUE(
      dao_->CreateAccount("Chase Checking", Account::Type::kChecking).ok());
  ASSERT_TRUE(dao_->DeleteAccount("Chase Checking").ok());

  EXPECT_TRUE(
      dao_->CreateAccount("Chase Checking", Account::Type::kSavings).ok());
}

TEST_F(AccountDaoTest, DeleteAccountLeavesOthers) {
  ASSERT_TRUE(dao_->CreateAccount("Checking", Account::Type::kChecking).ok());
  ASSERT_TRUE(dao_->CreateAccount("Savings", Account::Type::kSavings).ok());

  ASSERT_TRUE(dao_->DeleteAccount("Checking").ok());

  EXPECT_FALSE(dao_->GetAccount("Checking").ok());
  EXPECT_TRUE(dao_->GetAccount("Savings").ok());
}

}  // namespace

}  // namespace vigil
