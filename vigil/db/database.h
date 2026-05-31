#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include "pulse/core/error.h"
#include "pulse/core/result.h"
#include "sqlite3.h"
#include "vigil/db/sqlite_traits/column_extractor.h"
#include "vigil/db/sqlite_traits/parameter_binder.h"
#include "vigil/db/sqlite_traits/types.h"

namespace vigil {

class Database {
 public:
  static pulse::Result<Database> Open(std::string_view path);

  explicit Database() = default;

  ~Database() = default;

  // Movable and copyable.
  Database(const Database&) = default;
  Database& operator=(const Database&) = default;

  Database(Database&&) noexcept = default;
  Database& operator=(Database&&) noexcept = default;

  pulse::Result<void> Initialize();

  template <SqlCallback F = decltype([]() {})>
  pulse::Result<void> Execute(
      std::string_view sql,
      std::unordered_map<std::string, SqlValue> parameters = {},
      F&& callback = {});

 private:
  class Impl {
   public:
    explicit Impl(sqlite3* db) : db_(db) {}

    ~Impl() { (void)sqlite3_close(db_); }

    // Not copyable or movable.
    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;

    sqlite3* handle() const { return db_; }

   private:
    sqlite3* db_;
  };

  explicit Database(std::shared_ptr<Impl> db) : db_(std::move(db)) {}

  std::shared_ptr<Impl> db_;
};

// Implementation details below;

// TODO(map sqlite error codes to pulse::Error::Codes)
template <SqlCallback F>
pulse::Result<void> Database::Execute(
    std::string_view sql, std::unordered_map<std::string, SqlValue> parameters,
    F&& callback) {
  sqlite3_stmt* stmt;
  if (int err = sqlite3_prepare_v2(db_->handle(), sql.data(),
                                   static_cast<int>(sql.size()), &stmt,
                                   /*pzTail=*/nullptr);
      err != SQLITE_OK) {
    return pulse::Error{.code = pulse::Error::Code::kInternal,
                        .message = "sqlite3_prepare_v2 failed: " +
                                   std::string(sqlite3_errmsg(db_->handle()))};
  }

  for (const auto& [key, value] : parameters) {
    int i = sqlite3_bind_parameter_index(stmt, key.c_str());
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
      std::string msg = sqlite3_errmsg(db_->handle());
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
                                   std::string(sqlite3_errmsg(db_->handle()))};
  }

  return pulse::Result<void>{};
}

}  // namespace vigil
