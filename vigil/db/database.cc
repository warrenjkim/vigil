#include "vigil/db/database.h"

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "pulse/core/error.h"
#include "pulse/core/result.h"
#include "sqlite3.h"

namespace vigil {

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

}  // namespace vigil
