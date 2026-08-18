#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/services/conduit_service.h"

namespace vigil {

class ConduitSyncHandler final
    : public pulse::http::Handler<pulse::http::Method::kPost, "/accounts/sync",
                                  pulse::http::Dependencies<ConduitService*>> {
 public:
  explicit ConduitSyncHandler(ConduitService* conduit_service)
      : conduit_service_(*conduit_service) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  ConduitService& conduit_service_;
};

}  // namespace vigil
