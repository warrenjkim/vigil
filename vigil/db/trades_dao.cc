#include "vigil/db/trades_dao.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "pulse/core/result.h"
#include "pulse/core/stringify.h"
#include "vigil/db/database.h"
#include "vigil/db/time.h"
#include "vigil/db/trade.h"

namespace vigil {

TradesDao::TradesDao(Database db) : db_(db) {}

pulse::Result<void> TradesDao::CreateTrade(
    std::string_view account_name, Trade::Type type, std::string_view ticker,
    double shares, double price, std::optional<std::string_view> description) {
  return db_.Execute(
      R"sql(
        INSERT INTO Trades (
          AccountId,
          Type,
          Ticker,
          Shares,
          Price,
          Description,
          Timestamp
        )
        VALUES (
          (SELECT Id FROM Accounts WHERE Name = :account_name),
          :type,
          :ticker,
          :shares,
          :price,
          :description,
          :timestamp
        )
      )sql",
      /*parameters=*/{{":account_name", account_name},
                      {":type", pulse::to_string(type)},
                      {":ticker", ticker},
                      {":shares", shares},
                      {":price", price},
                      {":description", description},
                      {":timestamp", Time::Now().ToUnixSeconds()}});
}

pulse::Result<std::vector<Trade>> TradesDao::ListTrades(
    std::string_view account_name) {
  std::vector<Trade> trades;
  if (pulse::Result<void> err = db_.Execute(
          R"sql(
            SELECT
              t.Id,
              a.Name,
              t.Type,
              t.Ticker,
              t.Shares,
              t.Price,
              t.Description,
              t.Timestamp
            FROM
              Trades AS t
            JOIN Accounts AS a
            ON t.AccountId = a.Id
            WHERE
              a.Name = :account_name
          )sql",
          /*parameters=*/{{":account_name", account_name}},
          [&trades](int id, std::string account_name, std::string type,
                    std::string ticker, double shares, double price,
                    std::optional<std::string> description, int64_t timestamp) {
            trades.push_back(
                Trade{.id = id,
                      .account_name = std::move(account_name),
                      .type = to_trade_type(type),
                      .ticker = std::move(ticker),
                      .shares = shares,
                      .price = price,
                      .description = std::move(description),
                      .timestamp = Time::FromUnixSeconds(timestamp)});
          });
      !err.ok()) {
    return err.error();
  }

  return trades;
}

}  // namespace vigil
