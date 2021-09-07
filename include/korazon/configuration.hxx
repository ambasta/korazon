#ifndef KORAZON_CONFIGURATION
#define KORAZON_CONFIGURATION

#include <filesystem>

class Configuration {
public:
  bool load_file(std::filesystem::path );

  void bind(std::string_view, std::string_view);

  void bind(std::string_view, std::string_view, std::string_view);
};
#endif
