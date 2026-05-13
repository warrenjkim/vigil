#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/http/handler.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/handler_registry.h"

namespace vigil {

namespace {

using ::pulse::http::Handler;
using ::pulse::http::Request;
using ::pulse::http::Response;
using ::testing::Eq;
using ::testing::NotNull;

TEST(HealthHandlerTest, Test) {
  std::unique_ptr<Handler> handler = MakeHealthHandler();
  ASSERT_THAT(handler, NotNull());
  EXPECT_THAT((*handler)(Request{}),
              Eq(Response{.content_type = "application/json",
                          .status = 200,
                          .body = R"({"status": "ok"})"}));
}

}  // namespace

}  // namespace vigil
