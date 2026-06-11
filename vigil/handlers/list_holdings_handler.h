#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/holdings_dao.h"

namespace vigil {

class ListHoldingsHandler final : public pulse::http::Handler {
 public:
  PULSE_HTTP_ROUTE("/accounts/{name}/holdings", pulse::http::Method::kGet);
  using Dependencies = pulse::http::Dependencies<HoldingsDao*>;

  explicit ListHoldingsHandler(HoldingsDao* dao) : dao_(*dao) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  HoldingsDao& dao_;
};

}  // namespace vigil
