#include "vigil/handlers/api/merchant_aliases/list_merchant_aliases_handler.h"

#include <utility>
#include <vector>

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/core/stringify.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/json/value.h"
#include "vigil/db/merchant_alias.h"
#include "vigil/db/merchant_aliases_dao.h"

namespace vigil {

pulse::http::Response ListMerchantAliasesHandler::operator()(
    const pulse::http::Request&) const {
  pulse::Result<std::vector<MerchantAlias>> aliases = dao_.ListAliases();
  if (!aliases.ok()) {
    pulse::Log() << "ListMerchantAliases: list failed: "
                 << aliases.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "ListMerchantAliases: list succeeded (count="
               << aliases->size() << ", aliases=" << pulse::ToString(*aliases)
               << ")";

  pulse::json::Array response_body;
  response_body.reserve(aliases->size());
  for (auto& [merchant, display_name] : *aliases) {
    response_body.push_back(
        pulse::json::Object{{"merchant", std::move(merchant)},
                            {"display_name", std::move(display_name)}});
  }

  return pulse::http::Response{.content_type = "application/json",
                               .status = 200,
                               .body = pulse::ToString(response_body)};
}

}  // namespace vigil
