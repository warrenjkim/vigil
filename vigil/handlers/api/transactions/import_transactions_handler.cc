#include "vigil/handlers/api/transactions/import_transactions_handler.h"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "pulse/core/error.h"
#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/core/stringify.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/json/bind.h"
#include "pulse/json/parse.h"
#include "pulse/json/value.h"
#include "vigil/db/time.h"
#include "vigil/db/transaction.h"
#include "vigil/services/transaction_service.h"

namespace vigil {

namespace {

struct ImportTransactionRequest {
  std::string external_id;
  std::string type;
  double amount;
  std::string merchant;
  int64_t transaction_timestamp;

  static constexpr auto schema() {
    return pulse::json::Schema<ImportTransactionRequest>{}
        .Field("external_id", &ImportTransactionRequest::external_id)
        .Field("type", &ImportTransactionRequest::type)
        .Field("amount", &ImportTransactionRequest::amount)
        .Field("merchant", &ImportTransactionRequest::merchant)
        .Field("transaction_timestamp",
               &ImportTransactionRequest::transaction_timestamp);
  }
};

}  // namespace

ImportTransactionsHandler::ImportTransactionsHandler(
    TransactionService* transaction_service)
    : transaction_service_(*transaction_service) {}

pulse::http::Response ImportTransactionsHandler::operator()(
    const pulse::http::Request& request) const {
  auto account_name = request.path.Get<std::string>("name");
  if (!account_name.ok()) {
    pulse::Log() << "ImportTransactions: getting 'name': "
                 << account_name.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Result<pulse::json::Value> parsed = pulse::json::Parse(request.body);
  if (!parsed.ok() || !parsed->is<pulse::json::Array>()) {
    pulse::Log() << "ImportTransactions: parsing body: "
                 << (parsed.ok() ? "expected array" : parsed.error().message);
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  std::vector<Transaction> transactions;
  for (const auto& transaction : parsed->as<pulse::json::Array>()) {
    pulse::Result<ImportTransactionRequest> req =
        pulse::json::Bind<ImportTransactionRequest>(transaction);
    if (!req.ok()) {
      pulse::Log() << "ImportTransactions: binding transaction: "
                   << req.error().message;
      return pulse::http::Response{.content_type = "application/json",
                                   .status = 400,
                                   .body = R"({"status": "invalid argument"})"};
    }

    Transaction::Type transaction_type = ToTransactionType(req->type);
    if (transaction_type == Transaction::Type::kUnknown) {
      pulse::Log() << "ImportTransactions: unrecognized type '" << req->type
                   << "'";
      return pulse::http::Response{.content_type = "application/json",
                                   .status = 400,
                                   .body = R"({"status": "invalid argument"})"};
    }

    transactions.push_back({.external_id = std::move(req->external_id),
                            .type = transaction_type,
                            .amount = req->amount,
                            .merchant = std::move(req->merchant),
                            .transaction_timestamp = Time::FromUnixSeconds(
                                req->transaction_timestamp)});
  }

  pulse::Log() << "ImportTransactions: handling request (name='"
               << *account_name << "', count=" << transactions.size() << ")";

  pulse::Result<int> result =
      transaction_service_.ImportTransactions(*account_name, transactions);
  if (!result.ok()) {
    pulse::Log() << "ImportTransactions: import failed: "
                 << result.error().message;
    if (result.error().code == pulse::Error::Code::kNotFound) {
      return pulse::http::Response{.content_type = "application/json",
                                   .status = 404,
                                   .body = R"({"status": "not found"})"};
    }

    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "ImportTransactions: import succeeded (name='"
               << *account_name << "', count=" << transactions.size() << ")";

  return pulse::http::Response{
      .content_type = "application/json",
      .status = 200,
      .body = pulse::ToString(pulse::json::Object{{"imported", *result}})};
}

}  // namespace vigil
