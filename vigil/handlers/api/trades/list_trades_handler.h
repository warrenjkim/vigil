#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/trades_dao.h"

namespace vigil {

class ListTradesHandler final
    : public pulse::http::Handler<pulse::http::Method::kGet,
                                  "/accounts/{name}/trades",
                                  pulse::http::Dependencies<TradesDao*> > {
 public:
  explicit ListTradesHandler(TradesDao* dao) : dao_(*dao) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  TradesDao& dao_;
};

}  // namespace vigil
