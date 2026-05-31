#pragma once

#include <sqlite3.h>

#include <cstddef>
#include <string>
#include <string_view>

#include "vigil/db/sqlite_traits/types.h"

namespace vigil {

template <SqlType T>
struct ParameterBinder;

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

}  // namespace vigil
