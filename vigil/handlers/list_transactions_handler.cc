#include "vigil/handlers/list_transactions_handler.h"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/core/stringify.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/json/value.h"
#include "vigil/db/transaction.h"
#include "vigil/db/transactions_dao.h"

namespace vigil {

pulse::http::Response ListTransactionsHandler::operator()(
    const pulse::http::Request& request) const {
  auto name = request.path.Get<std::string>("name");
  if (!name.ok()) {
    pulse::Log() << "ListTransactions: getting 'name': "
                 << name.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Log() << "ListTransactions: handling request (name='" << *name << "')";

  pulse::Result<std::vector<Transaction>> transactions =
      dao_.ListTransactions(*name);
  if (!transactions.ok()) {
    pulse::Log() << "ListTransactions: list failed: "
                 << transactions.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "ListTransactions: list succeeded (name='" << *name
               << "', count=" << transactions->size()
               << ", transactions=" << pulse::ToString(*transactions);

  pulse::json::Array response_body;
  response_body.reserve(transactions->size());
  for (auto& [transaction_id, account_name, type, amount, description,
              timestamp] : *transactions) {
    response_body.push_back(pulse::json::Object{
        {"id", transaction_id},
        {"account_name", std::move(account_name)},
        {"type", pulse::ToString(type)},
        {"amount", amount},
        {"description", description.has_value() ? *std::move(description)
                                                : pulse::json::Value{nullptr}},
        {"timestamp", pulse::ToString(timestamp)}});
  }

  return pulse::http::Response{.content_type = "application/json",
                               .status = 200,
                               .body = pulse::ToString(response_body)};
}

}  // namespace vigil
