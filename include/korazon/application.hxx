#ifndef KORAZON_APPLICATION
#define KORAZON_APPLICATION

#include <giomm/settings.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>
#include <glibmm/variantdict.h>
#include <gtkmm/application.h>

class Application : public Gtk::Application {
private:
  const std::string m_log_tag = "application: ";
  bool m_running_as_service = false;

  void create_actions();

  void update_bypass_state(const Glib::ustring &);

protected:
  auto on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine> &)
      -> int override;

  auto on_handle_local_options(const Glib::RefPtr<Glib::VariantDict> &)
      -> int override;

  void on_startup() override;

  void on_activate() override;

public:
  Application();

  Application(const Application &) = delete;

  auto operator=(const Application &) -> Application & = delete;

  Application(const Application &&) = delete;

  auto operator=(const Application &&) -> Application & = delete;

  ~Application() override;

  static auto create() -> Glib::RefPtr<Application>;

  Glib::RefPtr<Gio::Settings> m_settings;
  Glib::RefPtr<Gio::Settings> m_soe_settings;
  Glib::RefPtr<Gio::Settings> m_sie_settings;
};

#endif
