#include <cstdint>
#include <korazon/korazon.hxx>
#include <korazon/utils.hxx>
#include <pipewire/context.h>
#include <pipewire/core.h>
#include <pipewire/keys.h>
#include <pipewire/main-loop.h>
#include <pipewire/node.h>
#include <pipewire/pipewire.h>
#include <pipewire/proxy.h>
#include <spa/utils/defs.h>
#include <spa/utils/dict.h>
#include <spa/utils/hook.h>
#include <utility>

std::string Korazon::name() const { return m_name; }

Korazon::Korazon() = default;

void Korazon::sync() {
  int pending = 0;
  spa_hook listener;
  spa_zero(listener);

  pw_core_events events{};
  events.version = PW_VERSION_CORE_EVENTS;
  events.done = [](void *data, uint32_t id, int seq) {
    auto &[client, pending] =
        *reinterpret_cast<std::pair<Korazon *, int *> *>(data);

    if (id == PW_ID_CORE and seq == *pending) {
      *pending = -1;
      pw_main_loop_quit(client->m_loop);
    }
  };
  events.error = [](void *data, uint32_t id, int seq, int result,
                    const char *msg) {
    auto &[client, pending] =
        *reinterpret_cast<std::pair<Korazon *, int *> *>(data);

    if (id == PW_ID_CORE and seq == *pending) {
      client->logger()->error("Core failure Seq: {} Result: {} Message: {}",
                              seq, result, msg);
      *pending = -1;
      pw_main_loop_quit(client->m_loop);
    }
  };

  auto data = std::make_pair(this, &pending);
  pw_core_add_listener(m_core, &listener, &events, &data);

  pending = pw_core_sync(m_core, PW_ID_CORE, 0);

  while (pending != -1)
    pw_main_loop_run(m_loop);
  spa_hook_remove(&listener);
}

void Korazon::on_global_removed(void *data, uint32_t id) {

  if (auto client = reinterpret_cast<Korazon *>(data); client) {
    if (id <= client->m_nodes.size())
      client->m_nodes[id - 1] = false;
  }
}

bool Korazon::setup() {
  Loggable::setup();
  pw_init(nullptr, nullptr);
  m_loop = pw_main_loop_new(nullptr);

  if (not m_loop) {
    logger()->error("Failed to create main loop");
    return false;
  }
  m_context = pw_context_new(pw_main_loop_get_loop(m_loop), nullptr, 0);

  if (not m_context) {
    logger()->error("Failed to get context");
    return false;
  }
  m_core = pw_context_connect(m_context, nullptr, 0);

  if (not m_core) {
    logger()->error("Unable to connect to context");
    return false;
  }
  m_registry = pw_core_get_registry(m_core, PW_VERSION_REGISTRY, 0);

  if (not m_registry) {
    logger()->error("Unable to get version registry");
    return false;
  }
  m_registry_events.global = on_global_added;
  m_registry_events.global_remove = on_global_removed;
  m_registry_events.version = PW_VERSION_REGISTRY_EVENTS;

  spa_zero(m_registry_listener);
  pw_registry_add_listener(m_registry, &m_registry_listener, &m_registry_events,
                           this);

  sync();

  if (m_nodes.empty())
    logger()->error("Unable to get nodes");
  return true;
}

Korazon::~Korazon() {

  if (m_registry)
    pw_proxy_destroy((pw_proxy *)m_registry);

  if (m_core)
    pw_core_disconnect(m_core);

  if (m_context)
    pw_context_destroy(m_context);

  if (m_loop)
    pw_main_loop_destroy(m_loop);
}

void Korazon::on_node_info(const pw_node_info *info) {
  uint32_t *node = nullptr;

  if (info) {
    const spa_dict_item *item;

    if (std::string_view media_class =
            spa_dict_lookup(info->props, "media.class");
        media_class.contains("Output/Audio") or
        media_class.contains("Audio/Sink")) {
      if (info->id > m_nodes.size())
        m_nodes.resize(info->id);
      logger()->info("Found audio output: {}: {}", info->id, media_class);
      m_nodes[info->id - 1] = true;
      m_done = true;
    } else {
      logger()->info("Unexpected media class: {}", media_class);
    }
  }
}

void Korazon::on_global_added(void *data, uint32_t id, uint32_t permissions,
                              const char *type, uint32_t version,
                              const spa_dict *properties) {
  auto *client = reinterpret_cast<Korazon *>(data);

  if (client and std::string_view{type} == PW_TYPE_INTERFACE_Node) {
    spa_hook listener;
    spa_zero(listener);

    pw_node_events node_events{};
    node_events.version = PW_VERSION_NODE_EVENTS;
    node_events.info = [](void *data, const pw_node_info *info) {
      auto *client = reinterpret_cast<Korazon *>(data);

      if (client)
        client->on_node_info(info);
    };

    auto *bound_node = reinterpret_cast<pw_node *>(pw_registry_bind(
        client->m_registry, id, type, PW_VERSION_NODE, sizeof(Korazon)));

    if (bound_node) {
      pw_node_add_listener(bound_node, &listener, &node_events, client);
      client->sync();
      spa_hook_remove(&listener);
      pw_proxy_destroy(reinterpret_cast<pw_proxy *>(bound_node));
    } else {
      client->logger()->info("No bind");
    }
  }
}

void Korazon::show() {
  while (not m_done)
    logger()->info("Sleeping");

  logger()->info("Status: ");
  for (auto index = 0; index < m_nodes.size(); ++index)
    if (m_nodes[index])
      logger()->info("Audio Node: {}", index);
}
