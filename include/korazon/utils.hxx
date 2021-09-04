#ifndef KORAZON_UTILS
#define KORAZON_UTILS

#include <functional>
#include <string>

constexpr inline auto operator""_sh(const char *string, size_t length) {
  return std::hash<std::string>{}(std::string(string, length));
}
#endif
