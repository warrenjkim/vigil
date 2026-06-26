#pragma once

#include "iris/iris_html.h"
#include "pulse/http/method.h"
#include "vigil/handlers/pages/static_handler.h"

namespace vigil {

using IndexHandler =
    StaticHandler<pulse::http::Method::kGet, "/", iris::kIrisHtml>;

}  // namespace vigil
