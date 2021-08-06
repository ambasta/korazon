cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project("" DESCRIPTION "" HOMEPAGE_URL "" LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

include(GenerateExportHeader)
include(CheckIPOSupported)
include(GNUInstallDirs)
include(GetVersionFromGit)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_GENERATOR Ninja)
set(LTO_SUPPORTED false CACHE BOOL "Compiler supports LTO")

option(ASAN "Enable address sanitizer" OFF)
option(TSAN "Enable thread sanitizer" OFF)
option(MSAN "Enable memory sanitizer" OFF)
option(UBSAN "Enable unbounded sanitizer" OFF)
option(ANAL "Enable code analysis" OFF)
option(BUILD_SHARED_LIBS "Build shared instead of static libraries" ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  string(APPEND CMAKE_CXX_FLAGS "-fno-omit-frame-pointer")
endif()

if(CMAKE_BUILD_TYPE MATCHES Release)
  string(APPEND CMAKE_CXX_FLAGS "-fwhole-program")
  string(APPEND CMAKE_CXX_FLAGS "-fgraphite-identity")
  string(APPEND CMAKE_CXX_FLAGS "-floop-nest-optimize")
  string(APPEND CMAKE_CXX_FLAGS "-fipa-pta")
  string(APPEND CMAKE_CXX_FLAGS "-fno-semantic-interposition")
  string(APPEND CMAKE_CXX_FLAGS "-fno-plt")
  check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT error)
endif()

if(ASAN AND TSAN AND MSAN AND UBSAN)
  message(FATAL_ERROR "Sanitizer options are mutually exclusive")
endif()

if(ASAN)
  string(APPEND CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address")
endif()

if(TSAN)
  string(APPEND CMAKE_CXX_FLAGS_DEBUG "-fsanitize=thread")
  string(APPEND CMAKE_CXX_FLAGS_DEBUG "-Wthread-safety")
endif()

if(MSAN)
  string(APPEND CMAKE_CXX_FLAGS_DEBUG "-fsanitize=memory")
endif()

if(UBSAN)
  string(APPEND CMAKE_CXX_FLAGS_DEBUG "-fsanitize=undefined")
endif()

if(ANAL)
  find_program(IWYU_PATH NAMES include-what-you-use iwyu REQUIRED)
  find_program(TIDY_PATH NAMES clang-tidy REQUIRED)
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE include-what-you-use)
  set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=*,readability-*)
  set(CMAKE_LINK_WHAT_YOU_USE ON)
endif()


# target_include_directories(${PROJECT_NAME} PUBLIC include)

# if(LTO_SUPPORTED)
#   set_property(
#     TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION
#     TRUE
#   )
# endif()

# set(PROJECT_NAME "x")
get_git_semver(PROJECT_VERSION)

file(WRITE "${PROJECT_BINARY_DIR}/VERSION" "${PROJECT_VERSION}")

# Install version
install(
  FILES
  "${PROJECT_BINARY_DIR}/VERSION"
  DESTINATION
  "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}"
  COMPONENT
  runtime
)

# Install cmake find script
install(
  FILES
  "${PROJECT_NAME}-config.cmake"
  DESTINATION
  "${CMAKE_INSTALL_DATADIR}/cmake/"
  COMPONENT
  dev
)

install(
  DIRECTORY
  ${PROJECT_SOURCE_DIR}/data
  DESTINATION
  ${CMAKE_INSTALL_LOCALSTATEDIR}/${PROJECT_NAME}
  COMPONENT
  runtime
)

install(
  DIRECTORY
  ${PROJECT_SOURCE_DIR}/config
  DESTINATION
  ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}
  COMPONENT
  runtime
)

add_subdirectory(src)
