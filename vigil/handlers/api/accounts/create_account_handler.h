#pragma once

#include <string>

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/reflect/schema.h"
#include "vigil/db/accounts_dao.h"

namespace vigil {

struct CreateAccountRequest {
  std::string name;
  std::string type;

  static constexpr auto Schema() {
    return pulse::reflect::Schema<CreateAccountRequest>{}
        .Field("name", &CreateAccountRequest::name)
        .Field("type", &CreateAccountRequest::type);
  }
};

class CreateAccountHandler final
    : public pulse::http::Handler<pulse::http::Method::kPost, "/accounts",
                                  pulse::http::Dependencies<AccountsDao*>,
                                  CreateAccountRequest> {
 public:
  using Request = CreateAccountRequest;

  explicit CreateAccountHandler(AccountsDao* dao) : dao_(*dao) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  AccountsDao& dao_;
};

}  // namespace vigil
