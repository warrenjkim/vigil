#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/services/account_service.h"

namespace vigil {

class ListAccountsHandler final
    : public pulse::http::Handler<pulse::http::Method::kGet, "/accounts",
                                  pulse::http::Dependencies<AccountService*> > {
 public:
  explicit ListAccountsHandler(AccountService* service) : service_(*service) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  AccountService& service_;
};

}  // namespace vigil
