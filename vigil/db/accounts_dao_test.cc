#include "vigil/db/accounts_dao.h"

#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "sqlite3.h"
#include "vigil/db/account.h"
#include "vigil/db/database.h"

namespace vigil {

namespace {

using ::testing::Eq;

class AccountsDaoTest : public ::testing::Test {
 protected:
  void SetUp() override {
    sqlite3_open(":memory:", &db_);
    Database database(db_);
    ASSERT_TRUE(database
                    .Execute(R"sql(
CREATE TABLE Accounts (
  Id INTEGER PRIMARY KEY AUTOINCREMENT,
  Name TEXT NOT NULL UNIQUE,
  Type TEXT NOT NULL
);
)sql")
                    .ok());
    dao = std::make_unique<AccountsDao>(database);
  }

  void TearDown() override { sqlite3_close(db_); }

  sqlite3* db_;
  std::unique_ptr<AccountsDao> dao;
};

TEST_F(AccountsDaoTest, InsertAndGet) {
  ASSERT_TRUE(
      dao->InsertAccount("Chase Checking", Account::Type::kChecking).ok());
  pulse::Result<Account> result = dao->GetAccount("Chase Checking");
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(*result, Eq(Account{.id = 1,
                                  .name = "Chase Checking",
                                  .type = Account::Type::kChecking}));
}

TEST_F(AccountsDaoTest, NotFound) {
  pulse::Result<Account> result = dao->GetAccount("nonexistent");
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.error().code, Eq(pulse::Error::Code::kNotFound));
}

TEST_F(AccountsDaoTest, DuplicateInsertFails) {
  ASSERT_TRUE(
      dao->InsertAccount("Chase Checking", Account::Type::kChecking).ok());
  pulse::Result<void> result =
      dao->InsertAccount("Chase Checking", Account::Type::kChecking);
  EXPECT_FALSE(result.ok());
}

}  // namespace

}  // namespace vigil
