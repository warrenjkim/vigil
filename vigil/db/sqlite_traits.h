#pragma once

#include <sqlite3.h>

#include <cstddef>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace vigil {

// TODO(expand as necessary)
template <typename T>
concept SqlType = std::is_same_v<T, int> || std::is_same_v<T, std::string>;

struct SqlValue : std::variant<int, std::string> {
  using variant::variant;

  SqlValue(std::string_view sv) : variant(std::string(sv)) {}
};

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

template <typename F>
struct Callback : Callback<decltype(&F::operator())> {};

template <typename C, typename R, typename... Args>
struct Callback<R (C::*)(Args...) const> {
  using ArgTypes = std::tuple<Args...>;
  using ReturnType = R;
};

template <typename F>
concept SqlCallback =
    std::is_same_v<typename Callback<F>::ReturnType, void> &&
    []<std::size_t... Is>(std::index_sequence<Is...>) {
      return (
          true && ... &&
          SqlType<std::tuple_element_t<Is, typename Callback<F>::ArgTypes>>);
    }(std::make_index_sequence<
        std::tuple_size_v<typename Callback<F>::ArgTypes>>{});

}  // namespace vigil
