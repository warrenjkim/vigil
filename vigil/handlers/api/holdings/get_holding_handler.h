#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/holdings_dao.h"

namespace vigil {

class GetHoldingHandler final
    : public pulse::http::Handler<pulse::http::Method::kGet,
                                  "/accounts/{name}/holdings/{ticker}",
                                  pulse::http::Dependencies<HoldingsDao*> > {
 public:
  explicit GetHoldingHandler(HoldingsDao* dao) : dao_(*dao) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  HoldingsDao& dao_;
};

}  // namespace vigil
