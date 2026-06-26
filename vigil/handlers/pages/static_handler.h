#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"

namespace vigil {

template <size_t N>
struct StringLiteral {
  constexpr StringLiteral(const char (&str)[N]) { std::copy_n(str, N, value); }

  consteval operator std::string_view() const {
    return std::string_view(value, N - 1);
  }

  operator std::string() const { return std::string(value, N - 1); }

  char value[N];
};

template <pulse::http::Method kHttpMethod, StringLiteral kFilePath,
          StringLiteral kContent, StringLiteral kContentType = "text/html">
class StaticHandler final : public pulse::http::Handler {
 public:
  PULSE_HTTP_ROUTE(kFilePath, kHttpMethod);
  using Dependencies = pulse::http::Dependencies<>;

  pulse::http::Response operator()(const pulse::http::Request&) const override {
    return pulse::http::Response{
        .content_type = kContentType, .status = 200, .body = kContent};
  }
};

}  // namespace vigil
