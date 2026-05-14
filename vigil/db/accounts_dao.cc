#include "vigil/db/accounts_dao.h"

#include <optional>

#include "pulse/core/stringify.h"
#include "vigil/db/account.h"
#include "vigil/db/database.h"

namespace vigil {

AccountsDao::AccountsDao(Database db) : db_(db) {}

pulse::Result<void> AccountsDao::InsertAccount(std::string_view name,
                                               Account::Type type) {
  if (pulse::Result<void> err = db_.Execute(
          R"sql(INSERT INTO Accounts (Name, Type) VALUES (:name, :type) )sql",
          {{":name", name}, {":type", pulse::to_string(type)}});
      !err.ok()) {
    return err.error();
  }

  return pulse::Result<void>{};
}

pulse::Result<Account> AccountsDao::GetAccount(std::string_view name) {
  std::optional<Account> account;
  if (pulse::Result<void> err = db_.Execute(
          R"sql(SELECT Id, Name, Type FROM Accounts WHERE Name = :name)sql",
          {{":name", name}},
          [&account](int id, std::string n, std::string type) {
            account =
                Account{.id = id, .name = n, .type = to_account_type(type)};
          });
      !err.ok()) {
    return err.error();
  } else if (!account.has_value()) {
    return pulse::Error{.code = pulse::Error::Code::kNotFound,
                        .message = "account not found: " + std::string(name)};
  }

  return *account;
}

}  // namespace vigil
