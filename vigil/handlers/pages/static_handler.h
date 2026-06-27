#pragma once

#include <cstddef>

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/strings/string_literal.h"

namespace vigil {

template <pulse::http::Method kHttpMethod,
          pulse::strings::StringLiteral kFilePath,
          pulse::strings::StringLiteral kContent,
          pulse::strings::StringLiteral kContentType = "text/html">
class StaticHandler final
    : public pulse::http::Handler<kHttpMethod, kFilePath> {
 public:
  pulse::http::Response operator()(const pulse::http::Request&) const override {
    return pulse::http::Response{
        .content_type = kContentType, .status = 200, .body = kContent};
  }
};

}  // namespace vigil
