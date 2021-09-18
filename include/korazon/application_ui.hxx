#ifndef KORAZON_APPLICATION_UI
#define KORAZON_APPLICATION_UI

#include <filesystem>
#include <glibmm/refptr.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/builder.h>
#include <korazon/application.hxx>
#include <sigc++/connection.h>

class ApplicationUi : public Gtk::ApplicationWindow {
private:
  const std::string log_tag = "application_ui: ";

  Application *m_application;

  Glib::RefPtr<Gio::Settings> m_settings;

  std::vector<sigc::connection> m_connections;

  static void apply_css_style(const std::string &);

public:
  ApplicationUi(BaseObjectType *, const Glib::RefPtr<Gtk::Builder> &,
                Application *);

  ApplicationUi(const ApplicationUi &) = delete;

  auto operator=(const ApplicationUi &) -> ApplicationUi & = delete;

  ApplicationUi(const ApplicationUi &&) = delete;

  auto operator=(const ApplicationUi &&) -> ApplicationUi & = delete;

  ~ApplicationUi() override;

  static auto create(Application *) -> ApplicationUi *;
};

#endif
