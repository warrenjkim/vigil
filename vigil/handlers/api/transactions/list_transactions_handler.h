#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/transactions_dao.h"

namespace vigil {

class ListTransactionsHandler final : public pulse::http::Handler {
 public:
  PULSE_HTTP_ROUTE("/accounts/{name}/transactions", pulse::http::Method::kGet);
  using Dependencies = pulse::http::Dependencies<TransactionsDao*>;

  explicit ListTransactionsHandler(TransactionsDao* dao) : dao_(*dao) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  TransactionsDao& dao_;
};

}  // namespace vigil
