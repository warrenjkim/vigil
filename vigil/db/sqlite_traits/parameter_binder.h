#pragma once

#include <sqlite3.h>

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include "vigil/db/sqlite_traits/types.h"

namespace vigil {

template <SqlExtractable T>
struct ParameterBinder;

template <>
struct ParameterBinder<std::monostate> {
  static int Bind(sqlite3_stmt* stmt, int i, std::monostate) {
    return sqlite3_bind_null(stmt, i);
  }
};

template <>
struct ParameterBinder<int> {
  static int Bind(sqlite3_stmt* stmt, int i, int value) {
    return sqlite3_bind_int(stmt, i, value);
  }
};

template <>
struct ParameterBinder<std::string> {
  static int Bind(sqlite3_stmt* stmt, int i, std::string_view value) {
    return sqlite3_bind_text(stmt, i, value.data(),
                             static_cast<int>(value.size()), SQLITE_TRANSIENT);
  }
};

template <SqlExtractable T>
struct ParameterBinder<std::optional<T>> {
  static int Bind(sqlite3_stmt* stmt, int i, std::optional<T> value) {
    return ParameterBinder<T>::Bind(
        stmt, i, value.has_value() ? *std::move(value) : std::monostate{});
  }
};

}  // namespace vigil
