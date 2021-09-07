#include <korazon/loggable.hxx>
#include <memory>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/common.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/systemd_sink.h>
#include <spdlog/spdlog.h>
#include <vector>

Loggable::Loggable() = default;

void Loggable::setup(const spdlog::level::level_enum &level) {
  auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  stdout_sink->set_level(level);

  auto systemd_sink = std::make_shared<spdlog::sinks::systemd_sink_mt>();
  systemd_sink->set_level(level);

  std::vector<spdlog::sink_ptr> sinks{stdout_sink, systemd_sink};
  auto logger = std::make_shared<spdlog::async_logger>(
      name(), sinks.begin(), sinks.end(), spdlog::thread_pool(),
      spdlog::async_overflow_policy::block);
  spdlog::register_logger(logger);
}

std::shared_ptr<spdlog::logger> Loggable::logger() const {
  return spdlog::get(name());
}
