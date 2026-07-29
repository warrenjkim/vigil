#pragma once

#include <cstdint>
#include <string>

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/reflect/schema.h"
#include "vigil/services/transaction_service.h"

namespace vigil {

struct ImportTransactionRequest {
  std::string external_id;
  std::string type;
  double amount;
  std::string merchant;
  int64_t transaction_timestamp;

  static constexpr auto Schema() {
    return pulse::reflect::Schema<ImportTransactionRequest>{}
        .Field("external_id", &ImportTransactionRequest::external_id)
        .Field("type", &ImportTransactionRequest::type)
        .Field("amount", &ImportTransactionRequest::amount)
        .Field("merchant", &ImportTransactionRequest::merchant)
        .Field("transaction_timestamp",
               &ImportTransactionRequest::transaction_timestamp);
  }
};

class ImportTransactionsHandler
    : public pulse::http::Handler<
          pulse::http::Method::kPost, "/accounts/{name}/transactions/import",
          pulse::http::Dependencies<TransactionService*>,
          ImportTransactionRequest> {
 public:
  explicit ImportTransactionsHandler(TransactionService* transaction_service);

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  TransactionService& transaction_service_;
};

}  // namespace vigil
