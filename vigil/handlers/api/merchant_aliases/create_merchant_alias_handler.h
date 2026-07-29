#pragma once

#include <string>

#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/reflect/schema.h"
#include "vigil/db/merchant_aliases_dao.h"

namespace vigil {

struct CreateMerchantAliasRequest {
  std::string merchant;
  std::string display_name;

  static constexpr auto Schema() {
    return pulse::reflect::Schema<CreateMerchantAliasRequest>{}
        .Field("merchant", &CreateMerchantAliasRequest::merchant)
        .Field("display_name", &CreateMerchantAliasRequest::display_name);
  }
};

class CreateMerchantAliasHandler final
    : public pulse::http::Handler<
          pulse::http::Method::kPost, "/merchant-aliases",
          pulse::http::Dependencies<MerchantAliasesDao*>,
          CreateMerchantAliasRequest> {
 public:
  explicit CreateMerchantAliasHandler(MerchantAliasesDao* dao) : dao_(*dao) {}

  pulse::http::Response operator()(
      const pulse::http::Request& request) const override;

 private:
  MerchantAliasesDao& dao_;
};

}  // namespace vigil
