#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/accounts_dao.h"

namespace vigil {

class DeleteAccountHandler final
    : public pulse::http::Handler<pulse::http::Method::kDelete,
                                  "/accounts/{name}",
                                  pulse::http::Dependencies<AccountsDao*>> {
 public:
  explicit DeleteAccountHandler(AccountsDao* dao) : dao_(*dao) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  AccountsDao& dao_;
};

}  // namespace vigil
