#ifndef KORAZON_PIPEWIRE
#define KORAZON_PIPEWIRE

#include <cstdint>
#include <korazon/extended_mutex.hxx>
#include <korazon/loggable.hxx>
#include <korazon/pipewire/details.hxx>
#include <optional>
#include <pipewire/core.h>
#include <pipewire/main-loop.h>
#include <pipewire/node.h>
#include <pipewire/port.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/*
class App {
  uint32_t m_pid;
  uint32_t m_node_id;
};
*/

class Pipewire : public Loggable {

private:
  const std::string m_name = "PipeWireWrapper";
  pw_core *m_core;
  pw_main_loop *m_loop;
  pw_context *m_context;
  pw_registry *m_registry;
  uint32_t m_version = 0;
  std::string m_default_speakers;
  spa_hook m_registry_listener;
  pw_registry_events m_registry_events;

  ExtendedMutex<std::unordered_map<uint32_t, Node>> m_nodes;
  ExtendedMutex<std::unordered_map<uint32_t, Port>> m_ports;

  std::unordered_map<std::string, std::vector<uint32_t>> audio_output_links;
  std::unordered_map<std::string, std::vector<uint32_t>>
      auduo_passthrough_links;

  void on_node_info(const pw_node_info *);

  void on_port_info(const pw_port_info *);

  void on_core_info(const pw_core_info *);

  void sync();

  bool create_null_source();

  bool delete_link(uint32_t);

  std::optional<int> link_ports(uint32_t, uint32_t);

  static void on_global_removed(void *, uint32_t);

  static void on_global_added(void *, uint32_t, uint32_t, const char *,
                              uint32_t, const spa_dict *);

  const std::string &name() const override;

protected:
  bool setup();

public:
  Pipewire();

  ~Pipewire();

  void destroy();

  bool use_as_default();

  bool revert_default();

  bool muteOutput(bool);

  std::unordered_set<std::string> current_output_applications();
  std::unordered_set<std::string> current_passedthrough();

  bool stop_all_passthrough();

  bool stop_passthrough(const std::string &);

  // bool passthrough_from(std::shared_ptr<App>);
  bool stop_audio_output();

  // bool output_sound_to(std::shared_ptr<App>);
  // std::vector<std::shared_ptr<App>> get_playing_apps();
  void run();
};

#endif
