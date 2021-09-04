add_executable(
  ${PROJECT_NAME} main.cxx loggable.cxx pipewire/details.cxx
  pipewire.cxx
)
# target_include_directories(
#   ${PROJECT_NAME} PUBLIC
#   ${PROJECT_BINARY_DIR}/third_party/include
# )
target_link_libraries(
  ${PROJECT_NAME}
  ${PROJECT_NAME}_HEADERS
  nlohmann_json::nlohmann_json
  spdlog::spdlog
  systemd::systemd
  Threads::Threads
)
