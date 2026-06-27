#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/transactions_dao.h"

namespace vigil {

class ListTransactionsHandler final
    : public pulse::http::Handler<
          pulse::http::Method::kGet, "/accounts/{name}/transactions",
          pulse::http::Dependencies<TransactionsDao*> > {
 public:
  explicit ListTransactionsHandler(TransactionsDao* dao) : dao_(*dao) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  TransactionsDao& dao_;
};

}  // namespace vigil
