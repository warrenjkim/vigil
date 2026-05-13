#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>

#include "pulse/dsa/result.h"
#include "sqlite3.h"
#include "vigil/db/sqlite_traits.h"

namespace vigil {

class Database {
 public:
  explicit Database(sqlite3* db) : db_(db) {}

  template <SqlCallback F = decltype([]() {})>
  pulse::Result<void> Execute(
      std::string_view sql,
      std::unordered_map<std::string, SqlValue> parameters = {},
      F&& callback = {});

 private:
  sqlite3* db_;
};

// Implementation details below;

template <SqlCallback F>
pulse::Result<void> Database::Execute(
    std::string_view sql, std::unordered_map<std::string, SqlValue> parameters,
    F&& callback) {
  sqlite3_stmt* stmt;
  if (int err = sqlite3_prepare_v2(db_, sql.data(), /*nByte=*/-1, &stmt,
                                   /*pzTail=*/nullptr);
      err != SQLITE_OK) {
    return pulse::Error{.code = pulse::Error::Code::kInternal,
                        .message = "sqlite3_prepare_v2 failed: " +
                                   std::string(sqlite3_errmsg(db_))};
  }

  for (const auto& [key, value] : parameters) {
    int i = sqlite3_bind_parameter_index(stmt, key.data());
    if (i == 0) {
      (void)sqlite3_finalize(stmt);
      return pulse::Error{
          .code = pulse::Error::Code::kInternal,
          .message = "no binding found for parameter '" + key + "'"};
    }

    if (int err = std::visit(
            [stmt, i](const auto& v) {
              return ParameterBinder<std::decay_t<decltype(v)>>::Bind(stmt, i,
                                                                      v);
            },
            value);
        err != SQLITE_OK) {
      std::string msg = sqlite3_errmsg(db_);
      (void)sqlite3_finalize(stmt);
      return pulse::Error{
          .code = pulse::Error::Code::kInternal,
          .message = "failed to bind parameter '" + key + "': " + msg};
    }
  }

  using Args = typename Callback<F>::ArgTypes;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    [&callback, stmt]<std::size_t... Is>(std::index_sequence<Is...>) {
      callback(
          ColumnExtractor<std::tuple_element_t<Is, Args>>::Get(stmt, Is)...);
    }(std::make_index_sequence<std::tuple_size_v<Args>>{});
  }

  if (int err = sqlite3_finalize(stmt); err != SQLITE_OK) {
    return pulse::Error{.code = pulse::Error::Code::kInternal,
                        .message = "sqlite3_finalize failed: " +
                                   std::string(sqlite3_errmsg(db_))};
  }

  return pulse::Result<void>{};
}

}  // namespace vigil
