#pragma once

#include "iris/accounts_new_html.h"
#include "pulse/http/method.h"
#include "vigil/handlers/pages/static_handler.h"

namespace vigil {

using GetNewAccountHandler =
    StaticHandler<pulse::http::Method::kGet, "/accounts/new",
                  iris::kAccountsNewHtml>;

}  // namespace vigil
