#include "vigil/handlers/create_account_handler.h"

#include <memory>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/core/result.h"
#include "pulse/core/result_or_die.h"
#include "pulse/core/stringify.h"
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

class CreateAccountHandlerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_ = pulse::unwrap_or_die(Database::Open(":memory:"));
    pulse::die_if_error(db_.Initialize());
    dao_ = std::make_unique<AccountsDao>(db_);

    ServerContext<AccountsDao*> ctx;
    ctx.set(dao_.get());
    router_ =
        pulse::unwrap_or_die(Router::Make<Routes<CreateAccountHandler>>(ctx));
  }

  Response RunMethod(Request req) {
    return (
        *router_
             .match(CreateAccountHandler::kMethod, CreateAccountHandler::kPath)
             ->handler)(std::move(req));
  }

  Database db_;
  std::unique_ptr<AccountsDao> dao_;
  Router router_;
};

TEST_F(CreateAccountHandlerTest, CreateAccount) {
  EXPECT_THAT(
      RunMethod(Request{
          .query = {{"name", "checking"},
                    {"type", pulse::to_string(Account::Type::kChecking)}}}),
      Eq(Response{
          .content_type = "text/plain", .status = 201, .body = "Created"}));
}

TEST_F(CreateAccountHandlerTest, PersistsAccount) {
  ASSERT_THAT(
      RunMethod(Request{.query = {{"name", "checking"},
                                  {"type", pulse::to_string(
                                               Account::Type::kChecking)}}})
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
      RunMethod(Request{.query = {{"type", pulse::to_string(
                                               Account::Type::kChecking)}}})
          .status,
      Eq(400));
}

TEST_F(CreateAccountHandlerTest, MissingTypeParam) {
  EXPECT_THAT(RunMethod(Request{.query = {{"name", "checking"}}}).status,
              Eq(400));
}

TEST_F(CreateAccountHandlerTest, UnrecognizedType) {
  EXPECT_THAT(
      RunMethod(Request{.query = {{"name", "checking"}, {"type", "bad_type"}}})
          .status,
      Eq(400));
}

TEST_F(CreateAccountHandlerTest, DuplicateCreate) {
  pulse::die_if_error(
      dao_->CreateAccount("checking", Account::Type::kChecking));
  EXPECT_THAT(
      RunMethod(Request{.query = {{"name", "checking"},
                                  {"type", pulse::to_string(
                                               Account::Type::kChecking)}}})
          .status,
      Eq(500));
}

}  // namespace

}  // namespace vigil
