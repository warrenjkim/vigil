#pragma once

#include <cstddef>
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
concept SqlType = std::is_same_v<T, int> || std::is_same_v<T, std::string>;

struct SqlValue : std::variant<int, std::string> {
  using variant::variant;

  SqlValue(std::string_view sv) : variant(std::string(sv)) {}
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
