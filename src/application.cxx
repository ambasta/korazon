#include <glibmm/i18n.h>
#include <glibmm/miscutils.h>
#include <gtkmm/aboutdialog.h>
#include <korazon/application.hxx>
#include <korazon/application_ui.hxx>

Application::Application()
    : Gtk::Application("com.github.ambasta.korazon",
                       Gio::Application::Flags::HANDLES_COMMAND_LINE) {
  Glib::set_application_name("Korazon");

  signal_handle_local_options().connect(
      sigc::mem_fun(*this, &Application::on_handle_local_options), false);

  add_main_option_entry(Gio::Application::OptionType::BOOL, "quit", 'q',
                        _("Quit korazon. Useful when running in service mode"));
}

Application::~Application() {}

auto Application::create() -> Glib::RefPtr<Application> {
  return Glib::RefPtr<Application>(new Application());
}

auto Application::on_command_line(
    const Glib::RefPtr<Gio::ApplicationCommandLine> &cmdline) -> int {
  const auto &options = cmdline->get_options_dict();

  if (options->contains("quit")) {
    for (const auto &window : get_windows())
      window->hide();
    quit();
  }

  activate();

  return Gtk::Application::on_command_line(cmdline);
}

void Application::on_startup() {
  Gtk::Application::on_startup();

  m_settings = Gio::Settings::create("com.github.ambasta.korazon");
  m_soe_settings =
      Gio::Settings::create("com.github.ambasta.korazon.streamoutputs");
  m_sie_settings =
      Gio::Settings::create("com.github.ambasta.korazon.streaminputs");

  if (static_cast<int>(get_flags() & Gio::Application::Flags::IS_SERVICE) != 0)
    m_running_as_service = true;

  create_actions();

  if (m_running_as_service) {
    hold();
  }
}

void Application::on_activate() {
  if (get_active_window() == nullptr) {
    auto *const window = ApplicationUi::create(this);

    add_window(*window);
    window->signal_close_request().connect(
        [&, window]() {
          int width = 0, height = 0;
          window->get_default_size(width, height);

          m_settings->set_int("window-width", width);
          m_settings->set_int("window-height", height);

          delete window;

          if (m_settings->get_boolean("shutdown-on-window-close"))
            release();
          return false;
        },
        false);
    window->show();
  }
}

auto Application::on_handle_local_options(
    const Glib::RefPtr<Glib::VariantDict> &options) -> int {
  if (not options) {
  }

  return -1;
}

void Application::create_actions() {
  add_action("about", [&]() {
    auto *const dialog = new Gtk::AboutDialog();

    dialog->set_program_name("Korazon");
    dialog->set_version("0.0");
    dialog->set_comments(_("Volume mixer for pipewire"));
    dialog->set_authors({"Amit Prakash Ambasta"});
    dialog->set_license(Gtk::License::GPL_3_0);
    dialog->set_website("https://github.com/ambasta/korazon");

    dialog->set_modal(true);
    dialog->set_transient_for(*get_active_window());
    dialog->set_hide_on_close(true);
    dialog->present();
  });
}
