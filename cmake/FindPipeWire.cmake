find_package(PkgConfig QUIET)

pkg_search_module(PKG_PipeWire QUIET libpipewire-0.3)
pkg_search_module(PKG_Spa QUIET libspa-0.2)

set(PipeWire_DEFINITIONS "${PKG_PipeWire_CFLAGS}" "${PKG_Spa_CFLAGS}")
set(PipeWire_VERSION "${PKG_PipeWire_VERSION}")

find_path(
  PipeWire_INCLUDE_DIRS
  NAMES
  pipewire/pipewire.h
  HINTS
  ${PKG_PipeWire_INCLUDE_DIRS}
  ${PKG_PipeWire_INCLUDE_DIRS}/pipewire-0.3
)

find_path(
  Spa_INCLUDE_DIRS
  NAMES
  spa/param/props.h
  HINTS
  ${PKG_Spa_INCLUDE_DIRS}
  ${PKG_Spa_INCLUDE_DIRS}/spa-0.2
)

find_library(
  PipeWire_LIBRARIES
  NAMES
  pipewire-0.3
  HINTS
  ${PKG_PipeWire_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  PipeWire
  FOUND_VAR
  PipeWire_FOUND
  REQUIRED_VARS
  PipeWire_LIBRARIES
  PipeWire_INCLUDE_DIRS
  Spa_INCLUDE_DIRS
  VERSION_VAR
  PipeWire_VERSION
)

if(PipeWire_FOUND AND NOT TARGET PipeWire::PipeWire)
  add_library(PipeWire::PipeWire UNKNOWN IMPORTED)
  message("Setting location to ${PipeWire_LIBRARIES}")
  set_target_properties(
    PipeWire::PipeWire
    PROPERTIES
    IMPORTED_LOCATION "${PipeWire_LIBRARIES}"
    INTERFACE_COMPILE_OPTIONS "${PipeWire_DEFINITIONS}"
    INTERFACE_INCLUDE_DIRECTORIES "${PipeWire_INCLUDE_DIRS};${Spa_INCLUDE_DIRS}"
  )
endif()

mark_as_advanced(PipeWire_LIBRARIES PipeWire_INCLUDE_DIRS)

include(FeatureSummary)
set_package_properties(
  PipeWire PROPERTIES
  URL "https://www.pipewire.org"
  DESCRIPTION "PipeWire - multimedia processing"
)
