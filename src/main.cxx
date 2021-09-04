#include <korazon/pipewire.hxx>
#include <spdlog/async.h>
#include <thread>

int main() {
  spdlog::init_thread_pool(8192, std::thread::hardware_concurrency());
  Pipewire client;
  client.run();
  return 0;
}
