#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/services/transaction_service.h"

namespace vigil {

class ImportTransactionsHandler
    : public pulse::http::Handler<
          pulse::http::Method::kPost, "/accounts/{name}/transactions/import",
          pulse::http::Dependencies<TransactionService*>> {
 public:
  explicit ImportTransactionsHandler(TransactionService* transaction_service);

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  TransactionService& transaction_service_;
};

}  // namespace vigil
