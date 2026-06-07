#include "vigil/handlers/create_transaction_handler.h"

#include <optional>
#include <string>
#include <utility>

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/json/bind.h"
#include "pulse/json/parse.h"
#include "pulse/json/value.h"
#include "vigil/db/transaction.h"
#include "vigil/db/transactions_dao.h"

namespace vigil {

namespace {

struct CreateTransactionRequest {
  std::string type;
  double amount;
  std::optional<std::string> description;

  static constexpr auto schema() {
    return pulse::json::Schema<CreateTransactionRequest>{}
        .Field("type", &CreateTransactionRequest::type)
        .Field("amount", &CreateTransactionRequest::amount)
        .Field("description", &CreateTransactionRequest::description);
  }
};

}  // namespace

pulse::http::Response CreateTransactionHandler::operator()(
    const pulse::http::Request& request) const {
  auto name = request.path.get<std::string>("name");
  if (!name.ok()) {
    pulse::Log() << "CreateTransaction: getting 'name': "
                 << name.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Result<pulse::json::value> body = pulse::json::parse(request.body);
  if (!body.ok()) {
    pulse::Log() << "CreateTransaction: parsing body: " << body.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  auto req = pulse::json::Bind<CreateTransactionRequest>(std::move(*body));
  if (!req.ok()) {
    pulse::Log() << "CreateTransaction: binding body: " << req.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  Transaction::Type transfer_type = to_transfer_type(req->type);
  if (transfer_type == Transaction::Type::kUnknown) {
    pulse::Log() << "CreateTransaction: unrecognized type '" << req->type
                 << "'";
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  pulse::Log() << "CreateTransaction: handling request (name='" << *name
               << "', type='" << req->type << "', amount='" << req->amount
               << "')";

  if (pulse::Result<void> result = dao_.CreateTransaction(
          *name, transfer_type, req->amount, req->description);
      !result.ok()) {
    pulse::Log() << "CreateTransaction: create failed: "
                 << result.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 500,
                                 .body = R"({"status": "internal"})"};
  }

  pulse::Log() << "CreateTransaction: create succeeded (name='" << *name
               << "')";

  return pulse::http::Response{
      .content_type = "text/plain", .status = 201, .body = "Created"};
}

}  // namespace vigil
