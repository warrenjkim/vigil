#include "vigil/db/database.h"

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "pulse/core/error.h"
#include "pulse/core/result.h"
#include "pulse/strings/cat.h"
#include "sqlite3.h"
#include "vigil/db/schema_sql.h"

namespace vigil {

namespace {

constexpr int kSchemaVersion = 1;

}

pulse::Result<Database> Database::Open(std::string_view path) {
  sqlite3* raw = nullptr;
  if (int err = sqlite3_open_v2(std::string(path).c_str(), &raw,
                                SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                                /*zVfs=*/nullptr);
      err != SQLITE_OK) {
    std::string msg = raw ? sqlite3_errmsg(raw) : sqlite3_errstr(err);
    (void)sqlite3_close(raw);

    return pulse::Error{.code = pulse::Error::Code::kInternal,
                        .message = "sqlite3_open_v2 failed: " + msg};
  }

  Database db(std::make_shared<Impl>(raw));
  std::string journal_mode;
  if (pulse::Result<void> err =
          db.Execute("PRAGMA journal_mode = WAL;", /*parameters=*/{},
                     [&journal_mode](std::string mode) {
                       journal_mode = std::move(mode);
                     });
      !err.ok()) {
    return err.error();
  }

  if (journal_mode != "wal" && journal_mode != "memory") {
    return pulse::Error{
        .code = pulse::Error::Code::kInternal,
        .message = "journal_mode = WAL did not succeed. got: " + journal_mode};
  }

  if (pulse::Result<void> err = db.Execute("PRAGMA foreign_keys = ON;");
      !err.ok()) {
    return err.error();
  }

  return db;
}

pulse::Result<void> Database::Initialize() {
  int version = 0;
  if (pulse::Result<void> err =
          Execute("PRAGMA user_version;", /*parameters=*/{},
                  [&version](int v) { version = v; });
      !err.ok()) {
    return err.error();
  }

  if (version == kSchemaVersion) {
    return pulse::Result<void>{};
  }

  if (version != 0) {
    return pulse::Error{.code = pulse::Error::Code::kInternal,
                        .message = pulse::strings::Cat(
                            "unexpected schema version: ", version,
                            " (binary expects ", kSchemaVersion, ")")};
  }

  const std::string sql = pulse::strings::Cat(
      "BEGIN;\n", kSchemaSql, "\nPRAGMA user_version = ", kSchemaVersion,
      ";\nCOMMIT;");

  char* sql_err = nullptr;
  if (int err = sqlite3_exec(db_->handle(), sql.c_str(), /*callback=*/nullptr,
                             /*arg=*/nullptr, &sql_err);
      err != SQLITE_OK) {
    std::string msg = sql_err != nullptr ? sql_err : sqlite3_errstr(err);
    sqlite3_free(sql_err);
    (void)sqlite3_exec(db_->handle(), "ROLLBACK;", /*callback=*/nullptr,
                       /*arg=*/nullptr, /*errmsg=*/nullptr);
    return pulse::Error{
        .code = pulse::Error::Code::kInternal,
        .message = pulse::strings::Cat("schema initialization failed: ", msg)};
  }

  return pulse::Result<void>{};
}

int Database::Changes() const { return sqlite3_changes(db_->handle()); }

}  // namespace vigil
