#include "vigil/db/accounts_dao.h"

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "pulse/core/error.h"
#include "pulse/core/result.h"
#include "pulse/core/stringify.h"
#include "vigil/db/account.h"
#include "vigil/db/database.h"

namespace vigil {

AccountsDao::AccountsDao(Database db) : db_(db) {}

// TODO(return pulse::Error::Code::kAlreadyExists for duplicates)
pulse::Result<void> AccountsDao::CreateAccount(std::string_view name,
                                               Account::Type type) {
  return db_.Execute(
      R"sql(INSERT INTO Accounts (Name, Type) VALUES (:name, :type) )sql",
      {{":name", name}, {":type", pulse::ToString(type)}});
}

pulse::Result<Account> AccountsDao::GetAccount(std::string_view name) {
  std::optional<Account> account;
  if (pulse::Result<void> err = db_.Execute(
          R"sql(SELECT Id, Name, Type FROM Accounts WHERE Name = :name)sql",
          {{":name", name}},
          [&account](int id, std::string n, std::string type) {
            account = Account{.id = id, .name = n, .type = ToAccountType(type)};
          });
      !err.ok()) {
    return err.error();
  } else if (!account.has_value()) {
    return pulse::Error{.code = pulse::Error::Code::kNotFound,
                        .message = "account not found: " + std::string(name)};
  }

  return *account;
}

pulse::Result<std::vector<Account>> AccountsDao::ListAccounts() {
  std::vector<Account> accounts;
  if (pulse::Result<void> err = db_.Execute(
          R"sql(SELECT Id, Name, Type FROM Accounts)sql",
          /*parameters=*/{},
          [&accounts](int id, std::string name, std::string type) {
            accounts.push_back(Account{.id = id,
                                       .name = std::move(name),
                                       .type = ToAccountType(type)});
          });
      !err.ok()) {
    return err.error();
  }

  return accounts;
}

pulse::Result<void> AccountsDao::DeleteAccount(std::string_view name) {
  return db_.Execute(R"sql(DELETE FROM Accounts WHERE Name = :name)sql",
                     {{":name", name}});
}

}  // namespace vigil
