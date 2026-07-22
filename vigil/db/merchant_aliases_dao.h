#pragma once

#include <string_view>
#include <vector>

#include "pulse/core/result.h"
#include "vigil/db/database.h"
#include "vigil/db/merchant_alias.h"

namespace vigil {

class MerchantAliasesDao {
 public:
  explicit MerchantAliasesDao(Database db);

  pulse::Result<void> CreateAlias(std::string_view merchant,
                                  std::string_view display_name);

  pulse::Result<MerchantAlias> GetAlias(std::string_view merchant);

  pulse::Result<std::vector<MerchantAlias>> ListAliases();

  pulse::Result<void> DeleteAlias(std::string_view name);

 private:
  Database db_;
};

}  // namespace vigil
