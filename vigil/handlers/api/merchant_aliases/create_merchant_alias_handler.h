#pragma once

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "vigil/db/merchant_aliases_dao.h"

namespace vigil {

class CreateMerchantAliasHandler final
    : public pulse::http::Handler<
          pulse::http::Method::kPost, "/merchant-aliases",
          pulse::http::Dependencies<MerchantAliasesDao*> > {
 public:
  explicit CreateMerchantAliasHandler(MerchantAliasesDao* dao) : dao_(*dao) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  MerchantAliasesDao& dao_;
};

}  // namespace vigil
