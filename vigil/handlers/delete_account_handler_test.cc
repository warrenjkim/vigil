#include <memory>

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

class DeleteAccountHandlerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_ = pulse::unwrap_or_die(Database::Open(":memory:"));
    pulse::die_if_error(db_.Initialize());

    dao_ = std::make_unique<AccountsDao>(db_);
    handler_ = MakeDeleteAccountHandler(dao_.get());

    ASSERT_THAT(handler_, NotNull());
  }

  Database db_;
  std::unique_ptr<AccountsDao> dao_;
  std::unique_ptr<pulse::http::Handler> handler_;
};

TEST_F(DeleteAccountHandlerTest, MissingNameParam) {
  EXPECT_THAT((*handler_)(pulse::http::Request{}).status, Eq(400));
}

TEST_F(DeleteAccountHandlerTest, DeleteAccount) {
  pulse::die_if_error(
      dao_->CreateAccount("checking", Account::Type::kChecking));

  pulse::http::Response response =
      (*handler_)(pulse::http::Request{.path_params = {{"name", "checking"}}});

  EXPECT_THAT(response.status, Eq(204));
}

TEST_F(DeleteAccountHandlerTest, PersistsDeletion) {
  pulse::die_if_error(
      dao_->CreateAccount("checking", Account::Type::kChecking));

  ASSERT_THAT(
      (*handler_)(pulse::http::Request{.path_params = {{"name", "checking"}}})
          .status,
      Eq(204));

  EXPECT_FALSE(dao_->GetAccount("checking").ok());
}

TEST_F(DeleteAccountHandlerTest, DeleteNonexistentAccount) {
  EXPECT_THAT((*handler_)(pulse::http::Request{
                              .path_params = {{"name", "nonexistent"}}})
                  .status,
              Eq(204));
}

}  // namespace

}  // namespace vigil
