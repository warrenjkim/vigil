#pragma once

#include <cstddef>

#include "iris/accounts_detail_html.h"
#include "iris/accounts_new_html.h"
#include "iris/dashboard_html.h"
#include "iris/iris_html.h"
#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/http/router.h"
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

using StaticFileHandlers = pulse::http::Routes<
    StaticHandler<pulse::http::Method::kGet, "/", iris::kIrisHtml>,
    StaticHandler<pulse::http::Method::kGet, "/dashboard",
                  iris::kDashboardHtml>,
    StaticHandler<pulse::http::Method::kGet, "/accounts/new",
                  iris::kAccountsNewHtml>,
    StaticHandler<pulse::http::Method::kGet, "/accounts/view",
                  iris::kAccountsDetailHtml>>;

}  // namespace vigil
