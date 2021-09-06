#ifndef KORAZON_KORAZON
#define KORAZON_KORAZON

#include <cstdint>
#include <korazon/loggable.hxx>
#include <pipewire/core.h>
#include <pipewire/main-loop.h>
#include <pipewire/node.h>
#include <spa/utils/dict.h>
#include <spa/utils/hook.h>
#include <string>
#include <vector>

class Korazon : public Loggable {
  const std::string m_name = "Korazon";
  std::vector<bool> m_nodes;
  pw_core *m_core;
  pw_main_loop *m_loop;
  pw_context *m_context;
  pw_registry *m_registry;
  spa_hook m_registry_listener;
  pw_registry_events m_registry_events;
  bool m_done = false;

public:
  Korazon();

  ~Korazon();

  void sync();

  bool setup();

  std::string name() const;

  void on_node_info(const pw_node_info *);

  static void on_global_added(void *, uint32_t, uint32_t, const char *,
                              uint32_t, const spa_dict *);

  static void on_global_removed(void *, uint32_t);

  void show();
};

#endif
