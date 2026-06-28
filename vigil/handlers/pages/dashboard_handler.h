#pragma once

#include "iris/dashboard_html.h"
#include "pulse/http/method.h"
#include "vigil/handlers/pages/static_handler.h"

namespace vigil {

using DashboardHandler = StaticHandler<pulse::http::Method::kGet, "/dashboard",
                                       iris::kDashboardHtml>;

}  // namespace vigil
