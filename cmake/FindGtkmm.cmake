find_package(PkgConfig QUIET)

pkg_search_module(PKG_Gtkmm QUIET gtkmm-4.0)

set(Gtkmm_DEFINITIONS "${PKG_Gtkmm_CFLAGS_OTHER}")
set(Gtkmm_VERSION "${PKG_Gtkmm_VERSION}")

find_library(Gtkmm_LOCATION
    NAMES
    gtkmm-4.0
    HINTS
    ${PKG_Gtkmm_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  Gtkmm
  FOUND_VAR Gtkmm_FOUND
  REQUIRED_VARS Gtkmm_LOCATION PKG_Gtkmm_INCLUDE_DIRS PKG_Gtkmm_LIBRARIES
  VERSION_VAR PKG_Gtkmm_VERSION
)

if(Gtkmm_FOUND AND NOT TARGET Gtkmm::Gtkmm)
  add_library(Gtkmm::Gtkmm UNKNOWN IMPORTED)
  set_target_properties(
    Gtkmm::Gtkmm
    PROPERTIES
    IMPORTED_LOCATION "${Gtkmm_LOCATION}"
    INTERFACE_COMPILE_OPTIONS "${PKG_Gtkmm_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${PKG_Gtkmm_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${PKG_Gtkmm_LIBRARIES}"
  )
endif()

mark_as_advanced(Gtkmm_LOCATION PKG_Gtkmm_LIBRARIES PKG_Gtkmm_CFLAGS_OTHER PKG_Gtkmm_INCLUDE_DIRS)

include(FeatureSummary)
set_package_properties(
  Gtkmm::Gtkmm PROPERTIES
  URL "https://www.gtk.org"
  DESCRIPTION "The GTK toolkit"
)
