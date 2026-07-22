#include "vigil/db/merchant_aliases_dao.h"

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "pulse/core/error.h"
#include "pulse/core/result.h"
#include "pulse/strings/cat.h"
#include "vigil/db/database.h"
#include "vigil/db/merchant_alias.h"

namespace vigil {

MerchantAliasesDao::MerchantAliasesDao(Database db) : db_(db) {}

pulse::Result<void> MerchantAliasesDao::CreateAlias(
    std::string_view merchant, std::string_view display_name) {
  return db_.Execute(
      R"sql(INSERT INTO MerchantAliases (RawMerchant, DisplayName)
            VALUES (:merchant, :display_name))sql",
      {{":merchant", merchant}, {":display_name", display_name}});
}

pulse::Result<MerchantAlias> MerchantAliasesDao::GetAlias(
    std::string_view merchant) {
  std::optional<MerchantAlias> alias = std::nullopt;
  if (pulse::Result<void> err = db_.Execute(
          R"sql(SELECT RawMerchant, DisplayName
                FROM MerchantAliases
                WHERE RawMerchant = :merchant)sql",
          {{":merchant", merchant}},
          [&alias](std::string raw, std::string display) {
            alias = MerchantAlias{.merchant = std::move(raw),
                                  .display_name = std::move(display)};
          });
      !err.ok()) {
    return err.error();
  } else if (!alias.has_value()) {
    return pulse::Error{
        .code = pulse::Error::Code::kNotFound,
        .message = pulse::strings::Cat("merchant alias not found: ", merchant)};
  }

  return *std::move(alias);
}

pulse::Result<std::vector<MerchantAlias>> MerchantAliasesDao::ListAliases() {
  std::vector<MerchantAlias> aliases;
  if (pulse::Result<void> err = db_.Execute(
          R"sql(SELECT RawMerchant, DisplayName FROM MerchantAliases)sql",
          /*parameters=*/{},
          [&aliases](std::string raw, std::string display) {
            aliases.push_back(
                MerchantAlias{.merchant = std::move(raw),
                              .display_name = std::move(display)});
          });
      !err.ok()) {
    return err.error();
  }

  return aliases;
}

pulse::Result<void> MerchantAliasesDao::DeleteAlias(std::string_view merchant) {
  return db_.Execute(
      R"sql(DELETE FROM MerchantAliases WHERE RawMerchant = :merchant)sql",
      {{":merchant", merchant}});
}

}  // namespace vigil
