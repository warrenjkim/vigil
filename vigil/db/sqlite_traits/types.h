#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace vigil {

// TODO(add a vigil::nullable)
// TODO(use extensible traits instead of relying on a hardcoded concept)
// TODO(expand as necessary)
template <typename T>
concept SqlType = std::is_same_v<T, std::monostate> || std::is_same_v<T, int> ||
                  std::is_same_v<T, int64_t> || std::is_same_v<T, double> ||
                  std::is_same_v<T, std::string>;

template <typename T>
concept SqlExtractable =
    SqlType<T> || (requires { typename T::value_type; } &&
                   std::is_same_v<T, std::optional<typename T::value_type>>);

struct SqlValue
    : std::variant<std::monostate, int, int64_t, double, std::string> {
  using variant::variant;

  SqlValue(std::string_view sv) : variant(std::string(sv)) {}

  SqlValue(std::optional<std::string_view> sv)
      : variant(sv.has_value() ? variant(std::string(*sv))
                               : variant(std::monostate{})) {}

  template <SqlType T>
  SqlValue(std::optional<T> value)
      : variant(value.has_value() ? variant(*std::move(value))
                                  : variant(std::monostate{})) {}
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
    []<size_t... Is>(std::index_sequence<Is...>) {
      return (true && ... &&
              SqlExtractable<
                  std::tuple_element_t<Is, typename Callback<F>::ArgTypes>>);
    }(std::make_index_sequence<
        std::tuple_size_v<typename Callback<F>::ArgTypes>>{});

}  // namespace vigil
