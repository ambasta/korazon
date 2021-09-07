#include <filesystem>
#include <korazon/configuration.hxx>

bool Configuration::load_file(std::filesystem::path path) {
  path += "/korazon.conf";
  return true;
}
