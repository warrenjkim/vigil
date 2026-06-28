#pragma once

#include <optional>
#include <string_view>

#include "pulse/core/result.h"
#include "vigil/db/database.h"
#include "vigil/db/holdings_dao.h"
#include "vigil/db/time.h"
#include "vigil/db/trade.h"
#include "vigil/db/trades_dao.h"

namespace vigil {

class TradeService {
 public:
  TradeService(Database* db, TradesDao* trades_dao, HoldingsDao* holdings_dao);

  pulse::Result<void> RecordTrade(std::string_view account_name,
                                  Trade::Type type, std::string_view ticker,
                                  double shares, double price,
                                  std::optional<std::string_view> description,
                                  Time trade_timestamp);

 private:
  Database& db_;
  TradesDao& trades_dao_;
  HoldingsDao& holdings_dao_;
};

}  // namespace vigil
