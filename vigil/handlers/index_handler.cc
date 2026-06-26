#include "vigil/handlers/index_handler.h"

#include <string>

#include "iris/iris_html.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"

namespace vigil {

pulse::http::Response IndexHandler::operator()(
    const pulse::http::Request&) const {
  return pulse::http::Response{.content_type = "text/html",
                               .status = 200,
                               .body = std::string(iris::kIrisHtml)};
}

}  // namespace vigil
