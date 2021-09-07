#ifndef KORAZON_KORAZON
#define KORAZON_KORAZON

#include <korazon/configuration.hxx>
#include <korazon/loggable.hxx>

void init_colors();

class Korazon : public Loggable {
private:
  Configuration m_configuration;

  std::string name() const override;

public:
  void load_configuration();

  void run();
};

#endif
