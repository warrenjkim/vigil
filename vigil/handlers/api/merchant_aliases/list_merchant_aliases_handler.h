#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/merchant_aliases_dao.h"

namespace vigil {

class ListMerchantAliasesHandler final
    : public pulse::http::Handler<
          pulse::http::Method::kGet, "/merchant-aliases",
          pulse::http::Dependencies<MerchantAliasesDao*> > {
 public:
  explicit ListMerchantAliasesHandler(MerchantAliasesDao* dao) : dao_(*dao) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  MerchantAliasesDao& dao_;
};

}  // namespace vigil
