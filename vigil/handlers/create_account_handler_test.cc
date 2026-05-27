#include <memory>

#include "core/result.h"
#include "core/stringify.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/core/result_or_die.h"
#include "pulse/http/handler.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/account.h"
#include "vigil/db/accounts_dao.h"
#include "vigil/db/database.h"
#include "vigil/handlers/handler_registry.h"

namespace vigil {

namespace {

using ::testing::Eq;
using ::testing::NotNull;

class CreateAccountHandlerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_ = pulse::unwrap_or_die(Database::Open(":memory:"));
    pulse::die_if_error(db_.Initialize());

    dao_ = std::make_unique<AccountsDao>(db_);
    handler_ = MakeCreateAccountHandler(dao_.get());

    ASSERT_THAT(handler_, NotNull());
  }

  Database db_;
  std::unique_ptr<AccountsDao> dao_;
  std::unique_ptr<pulse::http::Handler> handler_;
};

TEST_F(CreateAccountHandlerTest, CreateAccount) {
  EXPECT_THAT(
      (*handler_)(pulse::http::Request{
          .query_params = {{"name", "checking"},
                           {"type",
                            pulse::to_string(Account::Type::kChecking)}}}),
      Eq(pulse::http::Response{
          .content_type = "text/plain", .status = 201, .body = "Created"}));
}

TEST_F(CreateAccountHandlerTest, PersistsAccount) {
  ASSERT_THAT(
      (*handler_)(
          pulse::http::Request{
              .query_params = {{"name", "checking"},
                               {"type",
                                pulse::to_string(Account::Type::kChecking)}}})
          .status,
      Eq(201));

  pulse::Result<Account> account = dao_->GetAccount("checking");
  ASSERT_TRUE(account.ok());
  EXPECT_THAT(*account, Eq(Account{.id = 1,
                                   .name = "checking",
                                   .type = Account::Type::kChecking}));
}

TEST_F(CreateAccountHandlerTest, MissingNameParam) {
  EXPECT_THAT(
      (*handler_)(
          pulse::http::Request{
              .query_params = {{"type",
                                pulse::to_string(Account::Type::kChecking)}}})
          .status,
      Eq(400));
}

TEST_F(CreateAccountHandlerTest, MissingTypeParam) {
  EXPECT_THAT(
      (*handler_)(pulse::http::Request{.query_params = {{"name", "checking"}}})
          .status,
      Eq(400));
}

TEST_F(CreateAccountHandlerTest, UnrecognizedType) {
  EXPECT_THAT(
      (*handler_)(pulse::http::Request{.query_params = {{"name", "checking"},
                                                        {"type", "bad_type"}}})
          .status,
      Eq(400));
}

TEST_F(CreateAccountHandlerTest, DuplicateCreate) {
  pulse::die_if_error(
      dao_->CreateAccount("checking", Account::Type::kChecking));
  EXPECT_THAT(
      (*handler_)(
          pulse::http::Request{
              .query_params = {{"name", "checking"},
                               {"type",
                                pulse::to_string(Account::Type::kChecking)}}})
          .status,
      Eq(500));
}

}  // namespace

}  // namespace vigil
