add_library(${PROJECT_NAME}_HEADERS INTERFACE)
target_include_directories(${PROJECT_NAME}_HEADERS PRIVATE INTERFACE .)
target_link_libraries(
  ${PROJECT_NAME}_HEADERS INTERFACE PipeWire::PipeWire
  spdlog::spdlog
)
