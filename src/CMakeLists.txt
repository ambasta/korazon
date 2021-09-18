add_executable(
  ${PROJECT_NAME}
  loggable.cxx
  configuration.cxx
  pipewire.cxx
  korazon.cxx
  application.cxx
  main.cxx
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CURSES_INCLUDE_DIRS})

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
  Gtkmm::Gtkmm
  Threads::Threads
  ${CURSES_LIBRARIES}
)
