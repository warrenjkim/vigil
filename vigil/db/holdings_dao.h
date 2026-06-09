#pragma once

#include <string_view>
#include <vector>

#include "pulse/core/result.h"
#include "vigil/db/database.h"
#include "vigil/db/holding.h"

namespace vigil {

class HoldingsDao {
 public:
  explicit HoldingsDao(Database db);

  pulse::Result<void> CreateHolding(std::string_view account_name,
                                    std::string_view ticker, double shares);

  pulse::Result<Holding> GetHolding(std::string_view account_name,
                                    std::string_view ticker);

  pulse::Result<std::vector<Holding>> ListHoldings(
      std::string_view account_name);

  pulse::Result<void> UpdateShares(std::string_view account_name,
                                   std::string_view ticker, double shares);

  pulse::Result<void> DeleteHolding(std::string_view account_name,
                                    std::string_view ticker);

 private:
  Database db_;
};

}  // namespace vigil
