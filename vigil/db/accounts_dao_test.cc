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

TEST_F(AccountDaoTest, InsertAndGet) {
  ASSERT_TRUE(
      dao_->InsertAccount("Chase Checking", Account::Type::kChecking).ok());

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

TEST_F(AccountDaoTest, DuplicateInsertFails) {
  ASSERT_TRUE(
      dao_->InsertAccount("Chase Checking", Account::Type::kChecking).ok());
  auto result = dao_->InsertAccount("Chase Checking", Account::Type::kChecking);
  EXPECT_FALSE(result.ok());
}

}  // namespace
}  // namespace vigil
