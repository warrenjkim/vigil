#include "vigil/db/holdings_dao.h"

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "pulse/core/error.h"
#include "pulse/core/result.h"
#include "vigil/db/database.h"
#include "vigil/db/holding.h"

namespace vigil {

HoldingsDao::HoldingsDao(Database db) : db_(db) {}

pulse::Result<void> HoldingsDao::CreateHolding(std::string_view account_name,
                                               std::string_view ticker,
                                               double shares) {
  return db_.Execute(
      R"sql(
        INSERT INTO Holdings (AccountId, Ticker, Shares)
        VALUES (
          (SELECT Id FROM Accounts WHERE Name = :account_name),
          :ticker,
          :shares
        )
      )sql",
      /*parameters=*/{{":account_name", account_name},
                      {":ticker", ticker},
                      {":shares", shares}});
}

pulse::Result<Holding> HoldingsDao::GetHolding(std::string_view account_name,
                                               std::string_view ticker) {
  std::optional<Holding> holding;
  if (pulse::Result<void> err = db_.Execute(
          R"sql(
            SELECT
              h.Id,
              a.Name,
              h.Ticker,
              h.Shares
            FROM
              Holdings AS h
            JOIN Accounts AS a
            ON h.AccountId = a.Id
            WHERE
              a.Name = :account_name AND h.Ticker = :ticker
          )sql",
          /*parameters=*/{{":account_name", account_name}, {":ticker", ticker}},
          [&holding](int id, std::string account_name, std::string ticker,
                     double shares) {
            holding = Holding{.id = id,
                              .account_name = std::move(account_name),
                              .ticker = std::move(ticker),
                              .shares = shares};
          });
      !err.ok()) {
    return err.error();
  } else if (!holding.has_value()) {
    return pulse::Error{
        .code = pulse::Error::Code::kNotFound,
        .message = "holding not found: " + std::string(account_name) + "/" +
                   std::string(ticker)};
  }

  return *holding;
}

pulse::Result<std::vector<Holding>> HoldingsDao::ListHoldings(
    std::string_view account_name) {
  std::vector<Holding> holdings;
  if (pulse::Result<void> err = db_.Execute(
          R"sql(
            SELECT
              h.Id,
              a.Name,
              h.Ticker,
              h.Shares
            FROM
              Holdings AS h
            JOIN Accounts AS a
            ON h.AccountId = a.Id
            WHERE
              a.Name = :account_name
          )sql",
          {{":account_name", account_name}},
          [&holdings](int id, std::string account_name, std::string ticker,
                      double shares) {
            holdings.push_back(Holding{.id = id,
                                       .account_name = std::move(account_name),
                                       .ticker = std::move(ticker),
                                       .shares = shares});
          });
      !err.ok()) {
    return err.error();
  }

  return holdings;
}

pulse::Result<void> HoldingsDao::UpdateShares(std::string_view account_name,
                                              std::string_view ticker,
                                              double shares) {
  return db_.Execute(
      R"sql(
        UPDATE Holdings
        SET Shares = :shares
        WHERE
          AccountId = (SELECT Id FROM Accounts WHERE Name = :account_name)
          AND Ticker = :ticker
      )sql",
      /*parameters=*/{{":account_name", account_name},
                      {":ticker", ticker},
                      {":shares", shares}});
}

pulse::Result<void> HoldingsDao::DeleteHolding(std::string_view account_name,
                                               std::string_view ticker) {
  return db_.Execute(
      R"sql(
        DELETE FROM Holdings
        WHERE AccountId = (SELECT Id FROM Accounts WHERE Name = :account_name)
          AND Ticker = :ticker
      )sql",
      /*parameters=*/{{":account_name", account_name}, {":ticker", ticker}});
}

}  // namespace vigil
