#include "vigil/db/transactions_dao.h"

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
#include "vigil/db/transaction.h"

namespace vigil {

namespace {

using ::testing::Eq;
using ::testing::SizeIs;
using ::testing::StrEq;

class TransactionsDaoTest : public ::testing::Test {
 protected:
  void SetUp() override {
    pulse::Result<Database> db = Database::Open(":memory:");
    ASSERT_TRUE(db.ok()) << db.error().message;
    db_.emplace(std::move(*db));

    pulse::Result<void> err = db_->Initialize();
    ASSERT_TRUE(err.ok()) << err.error().message;

    accounts_dao_.emplace(*db_);
    ASSERT_TRUE(
        accounts_dao_->CreateAccount("checking", Account::Type::kChecking)
            .ok());

    dao_.emplace(*db_);
  }

  std::optional<Database> db_;
  std::optional<AccountsDao> accounts_dao_;
  std::optional<TransactionsDao> dao_;
};

TEST_F(TransactionsDaoTest, CreateAndList) {
  ASSERT_TRUE(dao_->CreateTransaction(
                      /*account_name=*/"checking", Transaction::Type::kDeposit,
                      /*amount=*/100,
                      /*merchant=*/"",
                      /*transaction_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());

  pulse::Result<std::vector<Transaction>> result =
      dao_->ListTransactions(/*account_name=*/"checking");
  ASSERT_TRUE(result.ok()) << result.error().message;
  ASSERT_THAT(*result, SizeIs(1));
  EXPECT_THAT((*result)[0].account_name, StrEq("checking"));
  EXPECT_THAT((*result)[0].type, Eq(Transaction::Type::kDeposit));
  EXPECT_THAT((*result)[0].amount, Eq(100));
}

TEST_F(TransactionsDaoTest, CreateWithDescription) {
  ASSERT_TRUE(dao_->CreateTransaction(
                      /*account_name=*/"checking", Transaction::Type::kDeposit,
                      /*amount=*/100,
                      /*merchant=*/"initial deposit",
                      /*transaction_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());

  pulse::Result<std::vector<Transaction>> result =
      dao_->ListTransactions(/*account_name=*/"checking");
  ASSERT_TRUE(result.ok()) << result.error().message;
  ASSERT_THAT(*result, SizeIs(1));
  EXPECT_THAT((*result)[0].merchant, StrEq("initial deposit"));
}

TEST_F(TransactionsDaoTest, ListEmptyTransactions) {
  pulse::Result<std::vector<Transaction>> result =
      dao_->ListTransactions(/*account_name=*/"checking");
  ASSERT_TRUE(result.ok()) << result.error().message;
  EXPECT_THAT(*result, SizeIs(0));
}

TEST_F(TransactionsDaoTest, ListMultipleTransactions) {
  ASSERT_TRUE(dao_->CreateTransaction(
                      /*account_name=*/"checking", Transaction::Type::kDeposit,
                      /*amount=*/100,
                      /*merchant=*/"",
                      /*transaction_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());
  ASSERT_TRUE(dao_->CreateTransaction(
                      /*account_name=*/"checking",
                      Transaction::Type::kWithdrawal,
                      /*amount=*/50,
                      /*merchant=*/"",
                      /*transaction_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());

  pulse::Result<std::vector<Transaction>> result =
      dao_->ListTransactions(/*account_name=*/"checking");
  ASSERT_TRUE(result.ok()) << result.error().message;
  EXPECT_THAT(*result, SizeIs(2));
}

TEST_F(TransactionsDaoTest, ListTransactionsIsolatedByAccount) {
  ASSERT_TRUE(
      accounts_dao_->CreateAccount("savings", Account::Type::kSavings).ok());

  ASSERT_TRUE(dao_->CreateTransaction(
                      /*account_name=*/"checking", Transaction::Type::kDeposit,
                      /*amount=*/100,
                      /*merchant=*/"",
                      /*transaction_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());
  ASSERT_TRUE(dao_->CreateTransaction(
                      /*account_name=*/"savings", Transaction::Type::kDeposit,
                      /*amount=*/200,
                      /*merchant=*/"",
                      /*transaction_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());

  pulse::Result<std::vector<Transaction>> result =
      dao_->ListTransactions(/*account_name=*/"checking");
  ASSERT_TRUE(result.ok()) << result.error().message;
  ASSERT_THAT(*result, SizeIs(1));
  EXPECT_THAT((*result)[0].amount, Eq(100));
}

TEST_F(TransactionsDaoTest, CreateForNonexistentAccount) {
  pulse::Result<void> result = dao_->CreateTransaction(
      /*account_name=*/"nonexistent", Transaction::Type::kDeposit,
      /*amount=*/100,
      /*merchant=*/"",
      /*transaction_timestamp=*/Time::FromUnixSeconds(0));
  EXPECT_FALSE(result.ok());
}

TEST_F(TransactionsDaoTest, GetBalanceSingleDeposit) {
  ASSERT_TRUE(dao_->CreateTransaction(
                      /*account_name=*/"checking", Transaction::Type::kDeposit,
                      /*amount=*/100,
                      /*merchant=*/"",
                      /*transaction_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());

  pulse::Result<double> result = dao_->GetBalance(/*account_name=*/"checking");
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(*result, Eq(100));
}

TEST_F(TransactionsDaoTest, GetBalanceDepositsAndWithdrawals) {
  ASSERT_TRUE(dao_->CreateTransaction(
                      /*account_name=*/"checking", Transaction::Type::kDeposit,
                      /*amount=*/1000,
                      /*merchant=*/"",
                      /*transaction_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());
  ASSERT_TRUE(dao_->CreateTransaction(
                      /*account_name=*/"checking",
                      Transaction::Type::kWithdrawal,
                      /*amount=*/250, /*merchant=*/"",
                      /*transaction_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());
  ASSERT_TRUE(dao_->CreateTransaction(
                      /*account_name=*/"checking", Transaction::Type::kDeposit,
                      /*amount=*/500, /*merchant=*/"",
                      /*transaction_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());

  pulse::Result<double> result = dao_->GetBalance(/*account_name=*/"checking");
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(*result, Eq(1250));
}

TEST_F(TransactionsDaoTest, GetBalanceIsolatedByAccount) {
  ASSERT_TRUE(
      accounts_dao_->CreateAccount("savings", Account::Type::kSavings).ok());
  ASSERT_TRUE(dao_->CreateTransaction(
                      /*account_name=*/"checking", Transaction::Type::kDeposit,
                      /*amount=*/100, /*merchant=*/"",
                      /*transaction_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());
  ASSERT_TRUE(dao_->CreateTransaction(
                      /*account_name=*/"savings", Transaction::Type::kDeposit,
                      /*amount=*/999, /*merchant=*/"",
                      /*transaction_timestamp=*/Time::FromUnixSeconds(0))
                  .ok());

  pulse::Result<double> result = dao_->GetBalance(/*account_name=*/"checking");
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(*result, Eq(100));
}

TEST_F(TransactionsDaoTest, GetBalanceNonexistentAccount) {
  pulse::Result<double> result =
      dao_->GetBalance(/*account_name=*/"nonexistent");
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(*result, Eq(0));
}

}  // namespace

}  // namespace vigil
