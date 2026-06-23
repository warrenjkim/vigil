#include "vigil/handlers/delete_account_handler.h"

#include <memory>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/core/result_or_die.h"
#include "pulse/http/handler.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/http/router.h"
#include "vigil/db/account.h"
#include "vigil/db/accounts_dao.h"
#include "vigil/db/database.h"

namespace vigil {

namespace {

using ::pulse::http::Request;
using ::pulse::http::Response;
using ::pulse::http::Router;
using ::pulse::http::Routes;
using ::pulse::http::ServerContext;
using ::testing::Eq;

class DeleteAccountHandlerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_ = pulse::UnwrapOrDie(Database::Open(":memory:"));
    pulse::DieIfError(db_.Initialize());
    dao_ = std::make_unique<AccountsDao>(db_);

    ServerContext<AccountsDao*> ctx;
    ctx.set(dao_.get());
    router_ =
        pulse::UnwrapOrDie(Router::Make<Routes<DeleteAccountHandler>>(ctx));
  }

  Response RunMethod(Request req) {
    return (
        *router_
             .Match(DeleteAccountHandler::kMethod, DeleteAccountHandler::kPath)
             ->handler)(std::move(req));
  }

  Database db_;
  std::unique_ptr<AccountsDao> dao_;
  Router router_;
};

TEST_F(DeleteAccountHandlerTest, MissingNameParam) {
  EXPECT_THAT(RunMethod(Request{}).status, Eq(400));
}

TEST_F(DeleteAccountHandlerTest, DeleteAccount) {
  pulse::DieIfError(dao_->CreateAccount("checking", Account::Type::kChecking));
  EXPECT_THAT(RunMethod(Request{.path = {{"name", "checking"}}}).status,
              Eq(204));
}

TEST_F(DeleteAccountHandlerTest, PersistsDeletion) {
  pulse::DieIfError(dao_->CreateAccount("checking", Account::Type::kChecking));
  ASSERT_THAT(RunMethod(Request{.path = {{"name", "checking"}}}).status,
              Eq(204));
  EXPECT_FALSE(dao_->GetAccount("checking").ok());
}

TEST_F(DeleteAccountHandlerTest, DeleteNonexistentAccount) {
  EXPECT_THAT(RunMethod(Request{.path = {{"name", "nonexistent"}}}).status,
              Eq(204));
}

}  // namespace

}  // namespace vigil
