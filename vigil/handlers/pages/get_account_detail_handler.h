#pragma once

#include "iris/accounts_detail_html.h"
#include "pulse/http/method.h"
#include "vigil/handlers/pages/static_handler.h"

namespace vigil {

using GetAccountDetailHandler =
    StaticHandler<pulse::http::Method::kGet, "/accounts/view",
                  iris::kAccountsDetailHtml>;

}  // namespace vigil
