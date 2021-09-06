#ifndef KORAZON_LOGGABLE
#define KORAZON_LOGGABLE

#include <memory>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <string>

class Loggable {
protected:
  virtual std::string name() const = 0;

  void setup(const spdlog::level::level_enum &level = spdlog::level::info);

  std::shared_ptr<spdlog::logger> logger() const;

public:
  Loggable();
};

#endif
