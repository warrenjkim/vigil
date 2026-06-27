#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/transactions_dao.h"

namespace vigil {

class CreateTransactionHandler final
    : public pulse::http::Handler<
          pulse::http::Method::kPost, "/accounts/{name}/transactions",
          pulse::http::Dependencies<TransactionsDao*> > {
 public:
  explicit CreateTransactionHandler(TransactionsDao* transactions_dao)
      : dao_(*transactions_dao) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  TransactionsDao& dao_;
};

}  // namespace vigil
