#ifndef KORAZON_UTILS
#define KORAZON_UTILS

#include <functional>
#include <string>

template <typename Functor, typename... Args>
constexpr inline auto wrap_callback(Functor functor, Args &&...args) {
  return functor(std::forward(args)...);
}
#endif
