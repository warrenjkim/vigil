#include "vigil/handlers/create_transaction_handler.h"

#include <optional>
#include <string>

#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/transaction.h"
#include "vigil/db/transactions_dao.h"

namespace vigil {

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

  auto type = request.query.get<std::string>("type");
  if (!type.ok()) {
    pulse::Log() << "CreateTransaction: getting 'type': "
                 << type.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  auto amount = request.query.get<double>("amount");
  if (!amount.ok()) {
    pulse::Log() << "CreateTransaction: getting 'amount': "
                 << amount.error().message;
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  Transaction::Type transfer_type = to_transfer_type(*type);
  if (transfer_type == Transaction::Type::kUnknown) {
    pulse::Log() << "CreateTransaction: unrecognized type '" << *type << "'";
    return pulse::http::Response{.content_type = "application/json",
                                 .status = 400,
                                 .body = R"({"status": "invalid argument"})"};
  }

  auto description = request.query.get<std::string>("description");
  std::optional<std::string> parsed_description =
      description.ok() ? std::optional(*description) : std::nullopt;

  pulse::Log() << "CreateTransaction: handling request (name='" << *name
               << "', type='" << *type << "', amount='" << *amount << "')";

  if (pulse::Result<void> result = dao_.CreateTransaction(
          *name, transfer_type, *amount, parsed_description);
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
