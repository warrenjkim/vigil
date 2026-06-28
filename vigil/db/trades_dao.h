#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "pulse/core/result.h"
#include "vigil/db/database.h"
#include "vigil/db/time.h"
#include "vigil/db/trade.h"

namespace vigil {

class TradesDao {
 public:
  explicit TradesDao(Database db);

  pulse::Result<void> CreateTrade(std::string_view account_name,
                                  Trade::Type type, std::string_view ticker,
                                  double shares, double price,
                                  std::optional<std::string_view> description,
                                  Time trade_timestamp);

  pulse::Result<std::vector<Trade>> ListTrades(std::string_view account_name);

 private:
  Database db_;
};

}  // namespace vigil
