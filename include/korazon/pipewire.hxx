#ifndef KORAZON_PIPEWIRE
#define KORAZON_PIPEWIRE

#include <cstdint>
#include <korazon/loggable.hxx>
#include <pipewire/core.h>
#include <pipewire/main-loop.h>
#include <pipewire/node.h>
#include <shared_mutex>
#include <spa/utils/dict.h>
#include <spa/utils/hook.h>
#include <string>
#include <vector>

class Pipewire : public Loggable {
  const std::string m_name = "Pipewire";
  std::vector<bool> m_nodes;
  pw_core *m_core;
  pw_main_loop *m_loop;
  pw_context *m_context;
  pw_registry *m_registry;
  spa_hook m_registry_listener;
  pw_registry_events m_registry_events;
  std::shared_mutex m_mutex;

public:
  Pipewire();

  ~Pipewire();

  void sync();

  static void on_global_removed(void *, uint32_t);

  void on_node_info(const pw_node_info *);

  static void on_global_added(void *, uint32_t, uint32_t, const char *,
                              uint32_t, const spa_dict *);

  bool setup();

  std::string name() const;

  void show();
};

#endif
