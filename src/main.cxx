#include <iostream>
#include <korazon/korazon.hxx>
#include <spdlog/async.h>
#include <thread>

int main(int argc, char *argv[]) {
  spdlog::init_thread_pool(8192, std::thread::hardware_concurrency());
  Korazon client;
  client.setup();
  client.show();
  /*
  Pipewire client;
  client.run();

  if (argc > 1) {
    int volume = std::stoi(argv[1]);

    if (volume == -1) {
      client.mute_output(true);
      std::cout << "Muted" << std::endl;
    } else if (volume > 100) {
      client.mute_output(false);
      std::cout << "Unmuted" << std::endl;
    } else {
      float vol_ratio = (float)volume / 100.0;
      client.set_volume(vol_ratio);
      std::cout << "Set volume to " << volume << std::endl;
    }
  }
  */
  return 0;
}
