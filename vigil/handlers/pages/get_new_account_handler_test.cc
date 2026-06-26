#include "vigil/handlers/pages/get_new_account_handler.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"

namespace vigil {

namespace {

using ::testing::Eq;
using ::testing::HasSubstr;

TEST(GetNewAccountHandlerTest, ReturnsHtml) {
  GetNewAccountHandler handler;
  pulse::http::Response response = handler(pulse::http::Request{});
  EXPECT_THAT(response.status, Eq(200));
  EXPECT_THAT(response.content_type, Eq("text/html"));
}

TEST(GetNewAccountHandlerTest, FormPostsToAccounts) {
  GetNewAccountHandler handler;
  pulse::http::Response response = handler(pulse::http::Request{});
  EXPECT_THAT(response.body, HasSubstr("action=\"/accounts\""));
  EXPECT_THAT(response.body, HasSubstr("method=\"post\""));
}

TEST(GetNewAccountHandlerTest, ContainsAccountTypeOptions) {
  GetNewAccountHandler handler;
  pulse::http::Response response = handler(pulse::http::Request{});
  EXPECT_THAT(response.body, HasSubstr("CHECKING"));
  EXPECT_THAT(response.body, HasSubstr("SAVINGS"));
  EXPECT_THAT(response.body, HasSubstr("BROKERAGE"));
  EXPECT_THAT(response.body, HasSubstr("401K"));
  EXPECT_THAT(response.body, HasSubstr("ROTH_IRA"));
  EXPECT_THAT(response.body, HasSubstr("TRADITIONAL_IRA"));
}

}  // namespace

}  // namespace vigil
