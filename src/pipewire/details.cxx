#include <korazon/pipewire/details.hxx>

uint32_t &Port::id() { return m_id; }
const uint32_t &Port::id() const { return m_id; }

std::string &Port::alias() { return m_alias; }
const std::string &Port::alias() const { return m_alias; }

spa_direction &Port::direction() { return m_direction; }
const spa_direction &Port::direction() const { return m_direction; }

Channel &Port::channel() { return m_channel; }
const Channel &Port::channel() const { return m_channel; }

uint32_t &Port::parent() { return m_parent; }
const uint32_t &Port::parent() const { return m_parent; }

uint32_t &Node::id() { return m_id; }
const uint32_t &Node::id() const { return m_id; }

uint32_t &Node::pid() { return m_pid; }
const uint32_t &Node::pid() const { return m_pid; }

std::string &Node::name() { return m_name; }
const std::string &Node::name() const { return m_name; }

std::string &Node::raw_name() { return m_raw_name; }
const std::string &Node::raw_name() const { return m_raw_name; }

std::string &Node::application() { return m_application; }
const std::string &Node::application() const { return m_application; }

bool &Node::is_monitor() { return m_is_monitor; }
const bool &Node::is_monitor() const { return m_is_monitor; }

std::unordered_map<uint32_t, Port> &Node::ports() { return m_ports; }
const std::unordered_map<uint32_t, Port> &Node::ports() const {
  return m_ports;
}
