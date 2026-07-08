#pragma once

#include <cstddef>

#include "iris/accounts_detail_html.h"
#include "iris/accounts_new_html.h"
#include "iris/api_js.h"
#include "iris/dashboard_html.h"
#include "iris/dashboard_js.h"
#include "iris/domain_js.h"
#include "iris/format_js.h"
#include "iris/index_html.h"
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
          pulse::strings::StringLiteral kContentType>
class StaticHandler final
    : public pulse::http::Handler<kHttpMethod, kFilePath> {
 public:
  pulse::http::Response operator()(const pulse::http::Request&) const override {
    return pulse::http::Response{
        .content_type = kContentType, .status = 200, .body = kContent};
  }
};

using StaticSourceFiles = pulse::http::Routes<
    StaticHandler<pulse::http::Method::kGet, "/", iris::kIndexHtml,
                  "text/html">,
    StaticHandler<pulse::http::Method::kGet, "/dashboard", iris::kDashboardHtml,
                  "text/html">,
    StaticHandler<pulse::http::Method::kGet, "/accounts/new",
                  iris::kAccountsNewHtml, "text/html">,
    StaticHandler<pulse::http::Method::kGet, "/accounts/view",
                  iris::kAccountsDetailHtml, "text/html">>;

using StaticScriptFiles = pulse::http::Routes<
    StaticHandler<pulse::http::Method::kGet, "/static/domain.js",
                  iris::kDomainJs, "application/javascript">,
    StaticHandler<pulse::http::Method::kGet, "/static/api.js", iris::kApiJs,
                  "application/javascript">,
    StaticHandler<pulse::http::Method::kGet, "/static/format.js",
                  iris::kFormatJs, "application/javascript">,
    StaticHandler<pulse::http::Method::kGet, "/static/dashboard.js",
                  iris::kDashboardJs, "application/javascript">>;

using StaticFileHandlers =
    pulse::http::Routes<StaticSourceFiles, StaticScriptFiles>;
}  // namespace vigil
