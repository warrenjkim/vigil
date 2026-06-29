#include "vigil/services/transaction_service.h"

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
#include "vigil/db/time.h"
#include "vigil/db/transaction.h"
#include "vigil/db/transactions_dao.h"

namespace vigil {

namespace {

using ::testing::Eq;
using ::testing::SizeIs;

class TransactionServiceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_ = pulse::UnwrapOrDie(Database::Open(":memory:"));
    pulse::DieIfError(db_.Initialize());
    accounts_dao_.emplace(db_);
    pulse::DieIfError(accounts_dao_->CreateAccount(/*name=*/"checking",
                                                   Account::Type::kChecking));
    transactions_dao_.emplace(db_);
    service_.emplace(&db_, &*accounts_dao_, &*transactions_dao_);
  }

  Database db_;
  std::optional<AccountsDao> accounts_dao_;
  std::optional<TransactionsDao> transactions_dao_;
  std::optional<TransactionService> service_;
};

TEST_F(TransactionServiceTest, ImportEmptyBatch) {
  pulse::Result<int> result =
      service_->ImportTransactions(/*account_name=*/"checking", {});
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(*result, Eq(0));
}

TEST_F(TransactionServiceTest, ImportSingleTransaction) {
  const std::vector<Transaction> txns = {
      {.external_id = "ext_001",
       .type = Transaction::Type::kDeposit,
       .amount = 100,
       .transaction_timestamp = Time::FromUnixSeconds(0)}};

  pulse::Result<int> result =
      service_->ImportTransactions(/*account_name=*/"checking", txns);
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(*result, Eq(1));

  pulse::Result<std::vector<Transaction>> listed =
      transactions_dao_->ListTransactions(/*account_name=*/"checking");
  ASSERT_TRUE(listed.ok());
  ASSERT_THAT(*listed, SizeIs(1));
  EXPECT_THAT((*listed)[0].type, Eq(Transaction::Type::kDeposit));
  EXPECT_THAT((*listed)[0].amount, Eq(100));
}

TEST_F(TransactionServiceTest, ImportMultipleTransactions) {
  const std::vector<Transaction> txns = {
      {.external_id = "ext_001",
       .type = Transaction::Type::kDeposit,
       .amount = 100,
       .transaction_timestamp = Time::FromUnixSeconds(0)},
      {.external_id = "ext_002",
       .type = Transaction::Type::kWithdrawal,
       .amount = 50,
       .merchant = "test",
       .transaction_timestamp = Time::FromUnixSeconds(0)},
      {.external_id = "ext_003",
       .type = Transaction::Type::kDeposit,
       .amount = 200,
       .transaction_timestamp = Time::FromUnixSeconds(0)}};

  pulse::Result<int> result =
      service_->ImportTransactions(/*account_name=*/"checking", txns);
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(*result, Eq(3));
}

TEST_F(TransactionServiceTest, ImportPartialDuplicates) {
  const std::vector<Transaction> first_batch = {
      {.external_id = "ext_001",
       .type = Transaction::Type::kDeposit,
       .amount = 100,
       .transaction_timestamp = Time::FromUnixSeconds(0)},
      {.external_id = "ext_002",
       .type = Transaction::Type::kDeposit,
       .amount = 200,
       .transaction_timestamp = Time::FromUnixSeconds(0)}};

  pulse::Result<int> first_result =
      service_->ImportTransactions(/*account_name=*/"checking", first_batch);
  ASSERT_TRUE(first_result.ok());
  EXPECT_THAT(*first_result, Eq(2));

  const std::vector<Transaction> second_batch = {
      {.external_id = "ext_002",
       .type = Transaction::Type::kDeposit,
       .amount = 200,
       .transaction_timestamp = Time::FromUnixSeconds(0)},
      {.external_id = "ext_003",
       .type = Transaction::Type::kWithdrawal,
       .amount = 50,
       .transaction_timestamp = Time::FromUnixSeconds(0)}};

  pulse::Result<int> second_result =
      service_->ImportTransactions(/*account_name=*/"checking", second_batch);
  ASSERT_TRUE(second_result.ok());
  EXPECT_THAT(*second_result, Eq(1));

  pulse::Result<std::vector<Transaction>> listed =
      transactions_dao_->ListTransactions(/*account_name=*/"checking");
  ASSERT_TRUE(listed.ok());
  EXPECT_THAT(*listed, SizeIs(3));
}

TEST_F(TransactionServiceTest, ImportIdempotent) {
  const std::vector<Transaction> txns = {
      {.external_id = "ext_001",
       .type = Transaction::Type::kDeposit,
       .amount = 100,
       .transaction_timestamp = Time::FromUnixSeconds(0)}};

  pulse::Result<int> first_result =
      service_->ImportTransactions(/*account_name=*/"checking", txns);
  ASSERT_TRUE(first_result.ok());
  EXPECT_THAT(*first_result, Eq(1));

  pulse::Result<int> second_result =
      service_->ImportTransactions(/*account_name=*/"checking", txns);
  ASSERT_TRUE(second_result.ok());
  EXPECT_THAT(*second_result, Eq(0));

  pulse::Result<std::vector<Transaction>> listed =
      transactions_dao_->ListTransactions(/*account_name=*/"checking");
  ASSERT_TRUE(listed.ok());
  EXPECT_THAT(*listed, SizeIs(1));
}

TEST_F(TransactionServiceTest, ImportWithMerchant) {
  const std::vector<Transaction> txns = {
      {.external_id = "ext_001",
       .type = Transaction::Type::kDeposit,
       .amount = 100,
       .merchant = "paycheck",
       .transaction_timestamp = Time::FromUnixSeconds(0)}};

  pulse::Result<int> result =
      service_->ImportTransactions(/*account_name=*/"checking", txns);
  ASSERT_TRUE(result.ok());
  EXPECT_THAT(*result, Eq(1));

  pulse::Result<std::vector<Transaction>> listed =
      transactions_dao_->ListTransactions(/*account_name=*/"checking");
  ASSERT_TRUE(listed.ok());
  ASSERT_THAT(*listed, SizeIs(1));
  EXPECT_THAT((*listed)[0].merchant, Eq("paycheck"));
}

TEST_F(TransactionServiceTest, ImportNonexistentAccountFails) {
  const std::vector<Transaction> txns = {
      {.external_id = "ext_001",
       .type = Transaction::Type::kDeposit,
       .amount = 100,
       .transaction_timestamp = Time::FromUnixSeconds(0)}};

  pulse::Result<int> result =
      service_->ImportTransactions(/*account_name=*/"nonexistent", txns);
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.error().code, Eq(pulse::Error::Code::kNotFound));
}

}  // namespace

}  // namespace vigil
