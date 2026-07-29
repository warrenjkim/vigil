#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "iris/console_css.h"
#include "iris/console_js.h"
#include "pulse/core/type_list.h"
#include "pulse/html/attributes.h"
#include "pulse/html/element.h"
#include "pulse/html/tags.h"
#include "pulse/http/handler.h"
#include "pulse/http/method.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"
#include "pulse/http/router.h"
#include "pulse/strings/cat.h"

namespace vigil {

namespace internal {

using ::pulse::html::A;
using ::pulse::html::Attributes;
using ::pulse::html::Body;
using ::pulse::html::Button;
using ::pulse::html::Class;
using ::pulse::html::Div;
using ::pulse::html::Element;
using ::pulse::html::H1;
using ::pulse::html::H2;
using ::pulse::html::Head;
using ::pulse::html::Hr;
using ::pulse::html::Href;
using ::pulse::html::Html;
using ::pulse::html::Id;
using ::pulse::html::Input;
using ::pulse::html::Make;
using ::pulse::html::Nav;
using ::pulse::html::OnClick;
using ::pulse::html::Placeholder;
using ::pulse::html::Pre;
using ::pulse::html::Script;
using ::pulse::html::Style;
using ::pulse::html::Table;
using ::pulse::html::Td;
using ::pulse::html::Title;
using ::pulse::html::Tr;

// TODO(move to pulse)
using Rel = ::pulse::html::internal::Attribute<"rel">;
using Link =
    ::pulse::html::Tag<"link", ::pulse::html::AttributeList<Href, Rel>>;

consteval size_t PathParamsSize(std::string_view path) {
  size_t size = 0;
  for (char c : path) {
    if (c == '{') {
      size++;
    }
  }

  return size;
}

template <size_t N>
consteval std::array<std::string_view, N> PathParams(std::string_view path) {
  std::array<std::string_view, N> params{};
  size_t i = 0;
  size_t pos = 0;
  while ((pos = path.find('{', pos)) != std::string::npos) {
    const size_t end = path.find('}', pos);
    params[i] = path.substr(pos + 1, end - pos - 1);
    pos = end + 1;
  }

  return params;
}

struct MethodInfo {
  std::string_view label;
  std::string_view js_fn;
};

inline constexpr MethodInfo GetMethodInfo(pulse::http::Method method) {
  switch (method) {
    case pulse::http::Method::kGet:
      return {.label = "GET", .js_fn = "get"};
    case pulse::http::Method::kPost:
      return {.label = "POST", .js_fn = "post"};
    case pulse::http::Method::kDelete:
      return {.label = "DELETE", .js_fn = "del"};
    default:
      return {.label = "?", .js_fn = "?"};
  }
}

template <typename Handler>
inline std::string FieldId(std::string_view param) {
  return pulse::strings::Cat(GetMethodInfo(Handler::kMethod).label, "-",
                             Handler::kPath, "-", param);
}

template <typename Handler>
std::string ResolvedPath() {
  std::string path(Handler::kPath);
  constexpr auto kParams =
      PathParams<PathParamsSize(Handler::kPath)>(Handler::kPath);
  for (std::string_view param : kParams) {
    const std::string placeholder = pulse::strings::Cat("{", param, "}");
    if (size_t pos = path.find(placeholder); pos != std::string::npos) {
      path.replace(
          pos, placeholder.size(),
          pulse::strings::Cat("${val('", FieldId<Handler>(param), "')}"));
    }
  }

  return path;
}

template <typename Handler>
constexpr auto RenderPathParamInputs() {
  constexpr size_t kParamsSize = PathParamsSize(Handler::kPath);
  constexpr std::array<std::string_view, kParamsSize> kParams =
      PathParams<kParamsSize>(Handler::kPath);
  return [&kParams]<std::size_t... I>(std::index_sequence<I...>) {
    return std::make_tuple(
        Make<Input>(Attributes{Id{FieldId<Handler>(kParams[I])},
                               Placeholder{std::string(kParams[I])}})...);
  }(std::make_index_sequence<kParamsSize>{});
}

template <typename Handler>
constexpr auto RenderBodyFieldInputs() {
  if constexpr (!std::same_as<typename Handler::HttpRequest, void>) {
    return Handler::HttpRequest::Schema().Map([](const auto& field) {
      return Make<Input>(Attributes{Id{FieldId<Handler>(field.key)},
                                    Placeholder{std::string(field.key)}});
    });
  } else {
    return std::tuple<>{};
  }
}

template <typename Handler>
constexpr auto RenderInputs() {
  return std::tuple_cat(RenderPathParamInputs<Handler>(),
                        RenderBodyFieldInputs<Handler>());
}

template <typename Handler>
constexpr auto BodyFieldFragments() {
  if constexpr (!std::same_as<typename Handler::HttpRequest, void>) {
    return Handler::HttpRequest::Schema().Map([](const auto& field) {
      if constexpr (std::is_arithmetic_v<typename std::remove_reference_t<
                        decltype(field)>::MemberFieldType>) {
        return pulse::strings::Cat(field.key, ": Number(val('",
                                   FieldId<Handler>(field.key), "'))");
      }

      return pulse::strings::Cat(field.key, ": val('",
                                 FieldId<Handler>(field.key), "')");
    });
  } else {
    return std::tuple<>{};
  }
}

template <typename... Fragments>
std::string JoinFragments(const std::tuple<Fragments...>& fragments) {
  return std::apply(
      [](const auto&... frags) {
        std::string joined;
        bool first = true;
        ((pulse::strings::Append(&joined, first ? "" : ", ", frags),
          first = false),
         ...);

        return joined;
      },
      fragments);
}

template <typename Handler>
Element RenderRoute() {
  constexpr MethodInfo info = GetMethodInfo(Handler::kMethod);
  const std::string label(info.label);

  std::string on_click;
  if constexpr (std::same_as<typename Handler::HttpRequest, void>) {
    on_click =
        pulse::strings::Cat(info.js_fn, "(`", ResolvedPath<Handler>(), "`)");
  } else {
    on_click = pulse::strings::Cat(
        info.js_fn, "(`", ResolvedPath<Handler>(), "`, { ",
        JoinFragments(BodyFieldFragments<Handler>()), " })");
  }

  return std::apply(
      [&](auto&&... inputs) {
        return Make<Tr>(
            Make<Td>(Attributes{Class{"path"}}, std::string(Handler::kPath)),
            Make<Td>(Make<Div>(
                Attributes{Class{"inputs"}}, std::move(inputs)...,
                Make<Button>(Attributes{Class{label}, OnClick{on_click}},
                             label))));
      },
      RenderInputs<Handler>());
}

template <typename... Handlers>
Element RenderTable(pulse::TypeList<Handlers...>) {
  return Make<Table>(RenderRoute<Handlers>()...);
}

template <typename Group>
Element RenderGroup() {
  return RenderTable(typename pulse::Flatten<pulse::TypeList<>, Group>::Type{});
}

template <typename... Groups>
Element RenderGroups(pulse::TypeList<Groups...>) {
  return Make<Div>(RenderGroup<Groups>()...);
}

template <typename... Groups>
std::string RenderPage(pulse::TypeList<Groups...> groups) {
  return pulse::html::RenderDocument(Make<Html>(
      Make<Head>(Make<Title>("Iris // Dev Console"),
                 Make<Link>(Attributes{
                     Rel{"stylesheet"},
                     Href{"https://cdn.jsdelivr.net/gh/dreampulse/"
                          "computer-modern-web-font@master/fonts.css"}}),
                 Make<Style>(std::string(iris::kConsoleCss))),
      Make<Body>(
          Make<Nav>(
              Attributes{Class{"nav"}},
              Make<A>(Attributes{Href{"/console"}, Class{"active"}}, "console"),
              Make<A>(Attributes{Href{"/dashboard"}}, "dashboard")),
          Make<H1>("Iris // Dev Console"), Make<Hr>(), RenderGroups(groups),
          Make<Div>(Attributes{Class{"section"}}, Make<H2>("response"),
                    Make<Pre>(Attributes{Id{"out"}}, "-")),
          Make<Script>(std::string(iris::kConsoleJs)))));
}

}  // namespace internal

template <pulse::http::HttpHandlers Handlers>
class ConsoleHandler final
    : public pulse::http::Handler<pulse::http::Method::kGet, "/console",
                                  pulse::http::Dependencies<>> {
 public:
  pulse::http::Response operator()(const pulse::http::Request&) const override {
    static const std::string kPage = internal::RenderPage(Handlers{});
    return pulse::http::Response{
        .content_type = "text/html", .status = 200, .body = kPage};
  }
};

}  // namespace vigil
