#pragma once

#include <sqlite3.h>

#include <cstddef>
#include <string>

#include "vigil/db/sqlite_traits/types.h"

namespace vigil {

template <SqlType T>
struct ColumnExtractor;

template <>
struct ColumnExtractor<int> {
  static int Get(sqlite3_stmt* stmt, size_t col) {
    return sqlite3_column_int(stmt, static_cast<int>(col));
  }
};

template <>
struct ColumnExtractor<std::string> {
  static std::string Get(sqlite3_stmt* stmt, size_t col) {
    return reinterpret_cast<const char*>(
        sqlite3_column_text(stmt, static_cast<int>(col)));
  }
};

}  // namespace vigil
