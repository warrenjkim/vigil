#include "vigil/handlers/api/accounts/conduit_sync_handler.h"

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/core/stringify.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/json/value.h"

namespace vigil {

pulse::http::Response ConduitSyncHandler::operator()(
    const pulse::http::Request&) const {
  pulse::Result<int> result = conduit_service_.Sync();
  if (!result.ok()) {
    pulse::Log() << "ConduitSyncHandler: sync failed: "
                 << result.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  return pulse::http::Response{
      .content_type = "application/json",
      .status = 200,
      .body = pulse::ToString(pulse::json::Object{{"imported", *result}})};
}

}  // namespace vigil
