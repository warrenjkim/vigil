#include "vigil/handlers/health_handler.h"

#include <optional>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/core/result.h"
#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/http/router.h"

namespace vigil {

namespace {

using ::pulse::http::Method;
using ::pulse::http::Request;
using ::pulse::http::Response;
using ::pulse::http::Router;
using ::pulse::http::Routes;
using ::pulse::http::ServerContext;
using ::testing::Eq;

TEST(HealthHandlerTest, ReturnsOk) {
  pulse::Result<Router> router =
      Router::Make<Routes<HealthHandler>>(ServerContext<>{});
  ASSERT_TRUE(router.ok());

  std::optional<Router::RouteMatch> match =
      router->Match(Method::kGet, "/health");
  ASSERT_TRUE(match.has_value());

  EXPECT_THAT((*match->handler)(Request{}),
              Eq(Response{.content_type = "application/json",
                          .status = 200,
                          .body = R"({"status": "ok"})"}));
}

}  // namespace

}  // namespace vigil
