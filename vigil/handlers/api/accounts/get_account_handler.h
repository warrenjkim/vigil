#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/accounts_dao.h"
#include "vigil/db/transactions_dao.h"

namespace vigil {

class GetAccountHandler final
    : public pulse::http::Handler<
          pulse::http::Method::kGet, "/accounts/{name}",
          pulse::http::Dependencies<AccountsDao*, TransactionsDao*> > {
 public:
  explicit GetAccountHandler(AccountsDao* accounts_dao,
                             TransactionsDao* transactions_dao)
      : accounts_dao_(*accounts_dao), transactions_dao_(*transactions_dao) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  AccountsDao& accounts_dao_;
  TransactionsDao& transactions_dao_;
};

}  // namespace vigil
