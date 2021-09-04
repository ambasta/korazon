#ifndef KORAZON_PIPEWIRE_DETAILS
#define KORAZON_PIPEWIRE_DETAILS

#include <cstdint>
#include <spa/utils/defs.h>
#include <string>
#include <unordered_map>

enum class Channel {
  UNDEFINED,
  LEFT,
  RIGHT,
  MONO,
};

class Port {
  uint32_t m_id;
  std::string m_alias;
  spa_direction m_direction;
  Channel m_channel = Channel::UNDEFINED;
  uint32_t m_parent = 0;

public:
  uint32_t &id();
  const uint32_t &id() const;

  std::string &alias();
  const std::string &alias() const;

  spa_direction &direction();
  const spa_direction &direction() const;

  Channel &channel();
  const Channel &channel() const;

  uint32_t &parent();
  const uint32_t &parent() const;
};

class Node {
  uint32_t m_id;
  std::string m_name;
  uint32_t m_pid;
  std::string m_raw_name;
  bool m_is_monitor = false;
  std::string m_application;
  std::unordered_map<uint32_t, Port> m_ports;

public:
  uint32_t &id();
  const uint32_t &id() const;

  std::string &name();
  const std::string &name() const;

  uint32_t &pid();
  const uint32_t &pid() const;

  std::string &raw_name();
  const std::string &raw_name() const;

  bool &is_monitor();
  const bool &is_monitor() const;

  std::string &application();
  const std::string &application() const;

  std::unordered_map<uint32_t, Port> &ports();
  const std::unordered_map<uint32_t, Port> &ports() const;
};

#endif
