#pragma once

#include <chrono>
#include <cstdint>
#include <ctime>
#include <string>

#include "pulse/core/stringify.h"

namespace vigil {

class Time {
 public:
  static Time Now() { return Time(std::chrono::system_clock::now()); }

  static Time FromUnixSeconds(int64_t s) {
    return Time(std::chrono::system_clock::time_point(std::chrono::seconds(s)));
  }

  int64_t ToUnixSeconds() const {
    return std::chrono::duration_cast<std::chrono::seconds>(
               tp_.time_since_epoch())
        .count();
  }

  friend bool operator==(const Time&, const Time&) = default;
  friend auto operator<=>(const Time&, const Time&) = default;

 private:
  explicit Time(std::chrono::system_clock::time_point tp) : tp_(tp) {}

  std::chrono::system_clock::time_point tp_;
};

}  // namespace vigil

template <>
struct pulse::Stringify<vigil::Time> {
  static std::string to_string(const vigil::Time& t) {
    time_t s = static_cast<time_t>(t.ToUnixSeconds());
    std::tm tm;
    gmtime_r(&s, &tm);
    char buf[21];  // "YYYY-MM-DDTHH:MM:SSZ\0"
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm);
    return buf;
  }
};
