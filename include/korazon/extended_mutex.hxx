#ifndef KORAZON_EXTENDED_MUTEX
#define KORAZON_EXTENDED_MUTEX

#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <utility>

template <typename T, bool ReadOnlyT, typename GuardT, typename... Args>
class Locked {
  using return_type = std::conditional_t<ReadOnlyT, const T &, T &>;

private:
  mutable GuardT m_lock;
  return_type m_value;

public:
  Locked(std::shared_mutex &mutex, return_type value, Args &&...args)
      : m_lock(mutex, std::forward(args)...), m_value(value) {}

  Locked(Locked &&) = delete;

  Locked(const Locked &) = delete;

  Locked &operator=(Locked &&) = delete;

  Locked &operator=(const Locked &) = delete;

  auto &operator*() const noexcept { return m_value; }

  auto &operator*() noexcept { return m_value; }

  auto *operator->() const noexcept { return &m_value; }

  auto *operator->() noexcept { return &m_value; }
};

template <typename T> class ExtendedMutex {
private:
  T m_value;
  mutable std::shared_mutex mutex;

public:
  template <typename... Args>
  explicit ExtendedMutex(Args &&...args) : m_value{std::forward(args)...} {}

  ExtendedMutex(ExtendedMutex &&) = delete;
  ExtendedMutex(const ExtendedMutex &) = delete;
  ExtendedMutex &operator=(ExtendedMutex &&) = delete;
  ExtendedMutex &operator=(const ExtendedMutex &) = delete;

  template <typename... Args> std::decay_t<T> copy(Args &&...args) const {
    return *Locked<T, true, std::shared_lock<std::shared_mutex>>(
        mutex, m_value, std::forward(args)...);
  }

  template <typename... Args> void assign(const T &value, Args &&...args) {
    *Locked<T, false, std::unique_lock<std::shared_mutex>>(
        mutex, m_value, std::forward(args)...) = value;
  }

  template <typename... Args> auto read(Args &&...args) const {
    return Locked<T, true, std::shared_lock<std::shared_mutex>>(
        mutex, m_value, std::forward(args)...);
  }

  template <typename... Args> auto write(Args &&...args) {
    return Locked<T, false, std::unique_lock<std::shared_mutex>>(
        mutex, m_value, std::forward(args)...);
  }

  auto &unsafe() { return m_value; }
};

#endif
