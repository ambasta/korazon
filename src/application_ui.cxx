#include <gtkmm/cssprovider.h>
#include <gtkmm/icontheme.h>
#include <korazon/application_ui.hxx>

ApplicationUi::ApplicationUi(BaseObjectType *cobject,
                             const Glib::RefPtr<Gtk::Builder> &builder,
                             Application *application)
    : Gtk::ApplicationWindow(cobject), m_application(application),
      m_settings(application->m_settings) {
  apply_css_style("custom.css");

  Gtk::IconTheme::get_for_display(Gdk::Display::get_default())
      ->add_resource_path("/com/github/ambasta/korazon/icons");
}

ApplicationUi::~ApplicationUi() {
  for (auto &connection : m_connections) {
    connection.disconnect();
  }
}

auto ApplicationUi::create(Application *application) -> ApplicationUi * {
  const auto &builder = Gtk::Builder::create_from_resource(
      "/com/github/ambasta/korazon/ui/application_window.ui");
  return Gtk::Builder::get_widget_derived<ApplicationUi>(
      builder, "ApplicationUi", application);
}

void ApplicationUi::apply_css_style(const std::string &css_filename) {
  auto provider = Gtk::CssProvider::create();

  provider->load_from_resource("/com/github/ambasta/korazon/ui/" +
                               css_filename);

  const auto &display = Gdk::Display::get_default();
  const auto &priority = GTK_STYLE_PROVIDER_PRIORITY_APPLICATION;

  Gtk::StyleContext::add_provider_for_display(display, provider, priority);
}
