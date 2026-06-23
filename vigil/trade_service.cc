#include "vigil/trade_service.h"

#include <optional>
#include <string_view>
#include <utility>

#include "pulse/core/error.h"
#include "pulse/core/result.h"
#include "pulse/strings/cat.h"
#include "vigil/db/database.h"
#include "vigil/db/holding.h"
#include "vigil/db/holdings_dao.h"
#include "vigil/db/trade.h"
#include "vigil/db/trades_dao.h"

namespace vigil {

TradeService::TradeService(Database* db, TradesDao* trades_dao,
                           HoldingsDao* holdings_dao)
    : db_(*db), trades_dao_(*trades_dao), holdings_dao_(*holdings_dao) {}

pulse::Result<void> TradeService::RecordTrade(
    std::string_view account_name, Trade::Type type, std::string_view ticker,
    double shares, double price, std::optional<std::string_view> description) {
  return WrapInTransaction(
      &db_,
      [this, account_name, type, ticker, shares, price,
       description]() -> pulse::Result<void> {
        if (pulse::Result<void> err = trades_dao_.CreateTrade(
                account_name, type, ticker, shares, price, description);
            !err.ok()) {
          return err.error();
        }

        switch (const pulse::Result<Holding> holding =
                    holdings_dao_.GetHolding(account_name, ticker);
                type) {
          case Trade::Type::kBuy: {
            if (holding.ok()) {
              return holdings_dao_.UpdateShares(account_name, ticker,
                                                holding->shares + shares);
            }

            if (holding.error().code != pulse::Error::Code::kNotFound) {
              return holding.error();
            }

            return holdings_dao_.CreateHolding(account_name, ticker, shares);
          }
          case Trade::Type::kSell: {
            if (!holding.ok()) {
              return pulse::Error{
                  .code = pulse::Error::Code::kFailedPrecondition,
                  .message = pulse::strings::Cat("no position in ", ticker)};
            }

            const double new_shares = holding->shares - shares;
            if (new_shares < 0) {
              return pulse::Error{
                  .code = pulse::Error::Code::kFailedPrecondition,
                  .message =
                      pulse::strings::Cat("insufficient shares in ", ticker)};
            }

            if (new_shares == 0) {
              return holdings_dao_.DeleteHolding(account_name, ticker);
            }

            return holdings_dao_.UpdateShares(account_name, ticker, new_shares);
          }
          case Trade::Type::kUnknown:
            std::unreachable();
        }
      });
}

}  // namespace vigil
