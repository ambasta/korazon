#include <korazon/pipewire/details.hxx>

auto Port::id() -> uint32_t & { return m_id; }
auto Port::id() const -> const uint32_t & { return m_id; }

auto Port::alias() -> std::string & { return m_alias; }
auto Port::alias() const -> const std::string & { return m_alias; }

auto Port::direction() -> spa_direction & { return m_direction; }
auto Port::direction() const -> const spa_direction & { return m_direction; }

auto Port::channel() -> Channel & { return m_channel; }
auto Port::channel() const -> const Channel & { return m_channel; }

auto Port::parent() -> uint32_t & { return m_parent; }
auto Port::parent() const -> const uint32_t & { return m_parent; }

auto Node::id() -> uint32_t & { return m_id; }
auto Node::id() const -> const uint32_t & { return m_id; }

auto Node::pid() -> uint32_t & { return m_pid; }
auto Node::pid() const -> const uint32_t & { return m_pid; }

auto Node::name() -> std::string & { return m_name; }
auto Node::name() const -> const std::string & { return m_name; }

auto Node::raw_name() -> std::string & { return m_raw_name; }
auto Node::raw_name() const -> const std::string & { return m_raw_name; }

auto Node::application() -> std::string & { return m_application; }
auto Node::application() const -> const std::string & { return m_application; }

auto Node::is_monitor() -> bool & { return m_is_monitor; }
auto Node::is_monitor() const -> const bool & { return m_is_monitor; }

auto Node::ports() -> std::unordered_map<uint32_t, Port> & { return m_ports; }
auto Node::ports() const -> const std::unordered_map<uint32_t, Port> & {
  return m_ports;
}
