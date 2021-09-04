#include "mpark/patterns/match.hpp"
#include <korazon/pipewire.hxx>
// #include <mpark/patterns.hpp>
#include <nlohmann/json.hpp>
#include <pipewire/context.h>
#include <pipewire/core.h>
#include <pipewire/impl-metadata.h>
#include <pipewire/main-loop.h>
#include <pipewire/pipewire.h>
#include <pipewire/proxy.h>
#include <spa/utils/dict.h>
#include <spdlog/spdlog.h>

Pipewire::Pipewire() = default;

const std::string &Pipewire::name() const { return m_name; }

void Pipewire::run() { setup(); }

void Pipewire::sync() {
  int pending = 0;
  spa_hook core_listener;
  spa_zero(core_listener);

  pw_core_events core_events = {};
  core_events.version = PW_VERSION_CORE_EVENTS;
  core_events.done = [](void *data, uint32_t id, int sequence) {
    auto &[client, pending] =
        *reinterpret_cast<std::pair<Pipewire *, int *> *>(data);

    if (id == PW_ID_CORE and sequence == *pending) {
      *pending = -1;
      pw_main_loop_quit(client->m_loop);
    }
  };
  core_events.error = [](void *data, uint32_t id, int sequence, int result,
                         const char *message) {
    auto &[client, pending] =
        *reinterpret_cast<std::pair<Pipewire *, int *> *>(data);

    if (id == PW_ID_CORE and sequence == *pending) {
      client->logger()->error("Core failure - Seq {} - Result {}: {}", sequence,
                              result, message);
      *pending = -1;
      pw_main_loop_quit(client->m_loop);
    }
  };

  auto data = std::make_pair(this, &pending);
  pw_core_add_listener(m_core, &core_listener, &core_events, &data);

  pending = pw_core_sync(m_core, PW_ID_CORE, 0);

  while (pending != -1)
    pw_main_loop_run(m_loop);
  spa_hook_remove(&core_listener);
}

bool Pipewire::setup() {
  Loggable::setup();

  pw_init(nullptr, nullptr);
  m_loop = pw_main_loop_new(nullptr);

  if (not m_loop) {
    logger()->error("Failed to create main loop");
    return false;
  }
  m_context = pw_context_new(pw_main_loop_get_loop(m_loop), nullptr, 0);

  if (not m_context) {
    logger()->error("Failed to create context");
    return false;
  }
  m_core = pw_context_connect(m_context, nullptr, 0);

  if (not m_core) {
    logger()->error("Failed to connect context");
    return false;
  }
  m_registry = pw_core_get_registry(m_core, PW_VERSION_REGISTRY, 0);

  if (not m_registry) {
    logger()->error("Failed to get registry");
    return false;
  }
  m_registry_events.global = on_global_added;
  m_registry_events.global_remove = on_global_removed;
  m_registry_events.version = PW_VERSION_REGISTRY_EVENTS;

  spa_zero(m_registry_listener);
  pw_registry_add_listener(m_registry, &m_registry_listener, &m_registry_events,
                           this);

  sync();

  if (m_default_speakers.empty()) {
    logger()->warn("Failed to retrieve default speakers");
  }
  return create_null_source();
  return true;
}

Pipewire::~Pipewire() {
  pw_proxy_destroy((pw_proxy *)m_registry);
  pw_core_disconnect(m_core);
  pw_context_destroy(m_context);
  pw_main_loop_destroy(m_loop);
}

void Pipewire::on_global_added(void *data, uint32_t id,
                               [[maybe_unused]] uint32_t permissions,
                               const char *type,
                               [[maybe_unused]] uint32_t version,
                               const spa_dict *properties) {
  auto *client = reinterpret_cast<Pipewire *>(data);

  if (client and properties) {
    /*
    using namespace mpark::patterns;

    match(type)(
        pattern(anyof(PW_TYPE_INTERFACE_Core)) =
            [] { spdlog::get("PWE")->info("Core matched"); },
        pattern(anyof(PW_TYPE_INTERFACE_Metadata)) =
            [] { spdlog::get("PWE")->info("Metadata matched"); });
    */

    if (strcmp(type, PW_TYPE_INTERFACE_Core) == 0) {
      spa_hook listener;
      spa_zero(listener);

      pw_core_events events = {};
      events.info = [](void *data, const pw_core_info *info) {
        auto *client = reinterpret_cast<Pipewire *>(data);
        if (client)
          client->on_core_info(info);
      };
      events.version = PW_VERSION_CORE_EVENTS;

      auto *bound_core = reinterpret_cast<pw_core *>(pw_registry_bind(
          client->m_registry, id, type, PW_VERSION_CORE, sizeof(Pipewire)));

      if (bound_core) {
        pw_core_add_listener(bound_core, &listener, &events, client);
        client->sync();
        spa_hook_remove(&listener);
        pw_proxy_destroy(reinterpret_cast<pw_proxy *>(bound_core));
      }
    }

    if (strcmp(type, PW_TYPE_INTERFACE_Metadata) == 0) {
      spa_hook listener;
      spa_zero(listener);

      pw_metadata_events events = {};
      events.property = [](void *data, [[maybe_unused]] uint32_t id,
                           const char *key, [[maybe_unused]] const char *type,
                           const char *value) -> int {
        auto *client = reinterpret_cast<Pipewire *>(data);

        if (client and key and value and
            key == std::string{"default.audio.sink"}) {
          auto parsed_value = nlohmann::json::parse(value, nullptr, false);

          if (not parsed_value.is_discarded() and parsed_value.count("name")) {
            auto name = parsed_value["name"].get<std::string>();
            client->m_default_speakers = name;
            client->logger()->info("Found default device: {}", name);
          }
        }
        return 0;
      };

      auto *bound_metadata = reinterpret_cast<pw_metadata *>(pw_registry_bind(
          client->m_registry, id, type, PW_VERSION_METADATA, sizeof(Pipewire)));

      if (bound_metadata) {
        pw_metadata_add_listener(bound_metadata, &listener, &events, client);
        client->sync();
        spa_hook_remove(&listener);
        pw_proxy_destroy(reinterpret_cast<pw_proxy *>(bound_metadata));
      }
    }
  }

  /*
  if (strcmp(type, PW_TYPE_INTERFACE_Node) == 0) {
    const auto *name = spa_dict_lookup(properties, PW_KEY_NODE_NAME);

    if (name and strstr(name, "kontol"))
      return;

    Node node;
    node.id() = id;
    node.raw_name() = name;

    spa_hook listener;
    spa_zero(listener);

    pw_node_events events = {};
    events.info = [](void *data, const pw_node_info *info) {
      auto *client = reinterpret_cast<Pipewire *>(data);
      if (client)
        client->on_node_info(info);
    };
    events.version = PW_VERSION_NODE_EVENTS;

    auto *bound_node = reinterpret_cast<pw_node *>(pw_registry_bind(
        client->m_registry, id, type, PW_VERSION_NODE, sizeof(Pipewire)));

    if (bound_node) {
      auto noderef = client->m_nodes.write();
      pw_node_add_listener(bound_node, &listener, &events, client);
      client->sync();
      spa_hook_remove(&listener);
      pw_proxy_destroy(reinterpret_cast<pw_proxy *>(bound_node));
    }
  }

  if (strcmp(type, PW_TYPE_INTERFACE_Port) == 0) {
    Port port;
    port.id() = id;

    spa_hook listener;
    spa_zero(listener);

    pw_port_events events = {};
    events.version = PW_VERSION_PORT_EVENTS;
    events.info = [](void *data, const pw_port_info *info) {
      auto *client = reinterpret_cast<Pipewire *>(data);

      if (client)
        client->on_port_info(info);
    };

    auto *bound_port = reinterpret_cast<pw_port *>(pw_registry_bind(
        client->m_registry, id, type, version, sizeof(Pipewire)));

    if (bound_port) {
      {
        auto write_access = client->m_ports.write();
        write_access->emplace(id, port);
      }
      pw_port_add_listener(bound_port, &listener, &events, client);
      client->sync();
      spa_hook_remove(&listener);
      pw_proxy_destroy(reinterpret_cast<pw_proxy *>(bound_port));

      {
        auto write_nodes = client->m_nodes.write();
        auto write_ports = client->m_ports.write();

        if (write_ports->find(id) != write_ports->end()) {
          auto &port = write_ports->at(id);

          if (port.parent() > 0 and
              write_nodes->find(port.parent()) != write_nodes->end()) {
            auto &node = write_nodes->at(port.parent());
            node.ports().emplace(id, port);
            write_ports->erase(id);
          }
        }
      }
    }
  }
  */
}

void Pipewire::on_global_removed(void *data, uint32_t id) {
  auto client = reinterpret_cast<Pipewire *>(data);

  if (client) {
    {
      auto nodes = client->m_nodes.write();

      if (nodes->find(id) != nodes->end())
        nodes->erase(id);
    }
    {
      auto ports = client->m_ports.write();

      if (ports->find(id) != ports->end())
        ports->erase(id);
    }
  }
}

bool Pipewire::create_null_source() {
  pw_properties *properties = pw_properties_new(nullptr, nullptr);
  pw_properties_set(properties, PW_KEY_MEDIA_CLASS, "Audio/Source");
  pw_properties_set(properties, PW_KEY_NODE_NAME, "kontol_sink");
  pw_properties_set(properties, PW_KEY_FACTORY_NAME,
                    "support.null-audio-source");

  auto *proxy = reinterpret_cast<pw_proxy *>(
      pw_core_create_object(m_core, "adapter", PW_TYPE_INTERFACE_Node,
                            PW_VERSION_NODE, &properties->dict, 0));

  if (not proxy) {
    logger()->error("Failed to create null source node");
    pw_properties_free(properties);
    return false;
  }
  spa_hook listener;
  spa_zero(listener);
  bool success = false;

  pw_proxy_events link_event = {};
  link_event.version = PW_VERSION_PROXY_EVENTS;
  link_event.bound = [](void *data, [[maybe_unused]] uint32_t id) {
    *reinterpret_cast<bool *>(data) = true;
  };
  link_event.error = [](void *data, [[maybe_unused]] int sequence,
                        [[maybe_unused]] int result, const char *message) {
    spdlog::get("PipeWireWrapper")
        ->error("Failed to create null sink: {}", message);
    *reinterpret_cast<bool *>(data) = false;
  };
  pw_proxy_add_listener(proxy, &listener, &link_event, &success);
  sync();

  spa_hook_remove(&listener);
  pw_properties_free(properties);
  return success;
}

void Pipewire::on_core_info(const pw_core_info *info) {
  if (info and info->name and info->version and not m_version) {
    logger()->info("Connected to pipewire ({}) on version {}", info->name,
                   info->version);
    std::string formatted_version(info->version);

    formatted_version.erase(
        std::remove(formatted_version.begin(), formatted_version.end(), '.'),
        formatted_version.end());

    m_version = std::stoi(formatted_version);

    if (m_version < 334) {
      logger()->warn("Pipewire version: {} too old (expected >=0.3.34). "
                     "Might experience bugs/crashes",
                     m_version);
    }
  }
}

void Pipewire::on_node_info(const pw_node_info *info) {
  Node *node = nullptr;
  {
    auto nodes = m_nodes.write();
    if (info and nodes->find(info->id) != nodes->end())
      node = &(nodes->at(info->id));
  }

  if (node) {
    if (const auto *pid =
            spa_dict_lookup(info->props, "application.process.id");
        pid)
      node->pid() = std::stol(pid);

    if (const auto *monitor = spa_dict_lookup(info->props, "stream.monitor");
        monitor)
      node->is_monitor() = true;

    if (const auto *app_name = spa_dict_lookup(info->props, "application.name");
        app_name)
      node->name() = app_name;

    if (const auto *binary =
            spa_dict_lookup(info->props, "application.process.binary");
        binary)
      node->application() = binary;
  }
}

void Pipewire::on_port_info(const pw_port_info *info) {
  Port *port = nullptr;

  {
    auto ports = m_ports.write();

    if (info and ports->find(info->id) != ports->end())
      port = &(ports->at(info->id));
  }

  if (port) {
    port->direction() = info->direction;

    if (const auto *node_id = spa_dict_lookup(info->props, "node.id"); node_id)
      port->parent() = std::stol(node_id);

    if (const auto *raw_name = spa_dict_lookup(info->props, "port.name");
        raw_name) {
      auto name = std::string(raw_name);

      if (name.back() == '1' or name.back() == 'L')
        port->channel() = Channel::LEFT;

      if (name.back() == '2' or name.back() == 'R')
        port->channel() = Channel::RIGHT;

      if (name.find("MONO", name.size() - 4) != std::string::npos)
        port->channel() = Channel::MONO;
    }

    if (const auto *alias = spa_dict_lookup(info->props, "port.alias"); alias)
      port->alias() = std::string(alias);
  }
}
