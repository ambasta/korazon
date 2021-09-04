find_package(PkgConfig QUIET)

pkg_check_modules(PC_${CMAKE_FIND_PACKAGE_NAME} "libsystemd")

set(
  ${CMAKE_FIND_PACKAGE_NAME}_DEFINITIONS
  ${PC_${CMAKE_FIND_PACKAGE_NAME}_CFLAGS}
  ${PC_${CMAKE_FIND_PACKAGE_NAME}_CFLAGS_OTHER}
)

set(
  ${CMAKE_FIND_PACKAGE_NAME}_VERSION
  "${PC_${CMAKE_FIND_PACKAGE_NAME}_VERSION}"
)

find_path(
  ${CMAKE_FIND_PACKAGE_NAME}_INCLUDE_DIRS
  NAMES systemd/sd-daemon.h
  HINTS ${PC_${CMAKE_FIND_PACKAGE_NAME}_INCLUDE_DIRS}
  ${PC_${CMAKE_FIND_PACKAGE_NAME}_INCLUDE_DIRS}/*
)

find_library(
  ${CMAKE_FIND_PACKAGE_NAME}_LIBRARIES
  NAMES
  libsystemd
  systemd
  HINTS
  ${PC_${CMAKE_FIND_PACKAGE_NAME}_LIBRARY_DIRS}
  ${PC_${CMAKE_FIND_PACKAGE_NAME}_LIBRARY_DIRS}/*
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  ${CMAKE_FIND_PACKAGE_NAME}
  FOUND_VAR
  ${CMAKE_FIND_PACKAGE_NAME}_FOUND
  REQUIRED_VARS
  ${CMAKE_FIND_PACKAGE_NAME}_LIBRARIES
  ${CMAKE_FIND_PACKAGE_NAME}_INCLUDE_DIRS
  VERSION_VAR
  ${CMAKE_FIND_PACKAGE_NAME}_VERSION
)

if(${CMAKE_FIND_PACKAGE_NAME}_FOUND AND NOT TARGET ${CMAKE_FIND_PACKAGE_NAME}::${CMAKE_FIND_PACKAGE_NAME})
  add_library(${CMAKE_FIND_PACKAGE_NAME}::${CMAKE_FIND_PACKAGE_NAME} UNKNOWN IMPORTED)
  set_target_properties(
    ${CMAKE_FIND_PACKAGE_NAME}::${CMAKE_FIND_PACKAGE_NAME}
    PROPERTIES
    IMPORTED_LOCATION "${${CMAKE_FIND_PACKAGE_NAME}_LIBRARIES}"
    INTERFACE_COMPILE_OPTIONS "${${CMAKE_FIND_PACKAGE_NAME}_DEFINITIONS}"
    INTERFACE_INCLUDE_DIRECTORIES "${${CMAKE_FIND_PACKAGE_NAME}_INCLUDE_DIRS}"
  )
endif()

mark_as_advanced(${CMAKE_FIND_PACKAGE_NAME}_LIBRARIES ${CMAKE_FIND_PACKAGE_NAME}_INCLUDE_DIRS)
set_package_properties(${CMAKE_FIND_PACKAGE_NAME} PROPERTIES URL "https://www.freedesktop.org/wiki/Software/systemd/" DESCRIPTION "System and Service Manager")