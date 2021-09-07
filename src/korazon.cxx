#include <clocale>
#include <cstdlib>
#include <filesystem>
#include <fmt/format.h>
#include <korazon/korazon.hxx>
#include <ncurses.h>
#include <ranges>
#include <string_view>

std::string Korazon::name() const { return "Korazon"; }

auto init_colors() -> void {
  start_color();
  init_pair(1, COLOR_GREEN, 0);
  init_pair(2, COLOR_YELLOW, 0);
  init_pair(3, COLOR_RED, 0);
}

void Korazon::load_configuration() {
  std::string_view home = std::getenv("HOME");
  std::string_view xdg_config_home = std::getenv("XDG_CONFIG_HOME");

  std::filesystem::path path = xdg_config_home.empty()
                                   ? fmt::format("{}/.config", home)
                                   : xdg_config_home;
  if (m_configuration.load_file(path))
    return;

  std::string_view xdg_config_dirs = std::getenv("XDG_CONFIG_DIRS");
  xdg_config_dirs = xdg_config_dirs.empty() ? "/etc/xdg" : xdg_config_dirs;

  for (const std::string_view xdg_config_dir :
       std::views::split(xdg_config_dirs, std::string_view{":"})) {
    if (m_configuration.load_file(xdg_config_dir))
      return;
  }

  m_configuration.bind("q", "quit");
  m_configuration.bind("Key_F(1)", "select-tab", "2");
  m_configuration.bind("-", "add-volume", "-0.05");
  m_configuration.bind("KEY_LEFT", "add-volume", "-0.05");
  m_configuration.bind("+", "add-volume", "0.05");
  m_configuration.bind("KEY_RIGHT", "add-volume", "0.05");
}

void Korazon::run() {
  // handle_arguments? CLI11 Parser
  load_configuration();

  std::setlocale(LC_ALL, "");
  initscr();
  // ncurses
  initscr();
  init_colors();
  nodelay(stdscr, true);
  set_escdelay(25);
  curs_set(0);
  keypad(stdscr, true);
  meta(stdscr, true);
  noecho();
}
