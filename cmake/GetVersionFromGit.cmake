# - Returns a semantic version from git
#
# These functions force a re-configure on each git commit so that you can
# trust the values of the variables in your build system.
#
# get_git_semver(<VAR> [<additional arguments to git describe --tags> ...])
#
# Returns the result of git describe --tags on the source tree, and adjusting
# the output so that it tests false if an error occurs.
#
# Git tags should follow semantic versioning with an optional v prefix.
# So tags such as v<MAJOR>.<MINOR>.<PATCH> or <MAJOR>.<MINOR>.<PATCH>
# are supported.
# Additionally, a tweak version component is automatically generated depending on
# how ahead the current commit is against the latest tag.
# In case of a dirty commit, REV is replaced by the short SHA1 of the current tree.
#
# If the latest tag is a valid semver, it populates the following variables
# <VAR>_MAJOR - Major version integer
# <VAR>_MINOR - Minor version integer
# <VAR>_PATCH - Patch version integer
# <VAR>_TWEAK - Tweak version string
# <VAR>       - Full semver

include(GetGitRevisionDescription)

function(get_git_semver _var)
  git_describe_working_tree(GIT_TAGVER "--tags")

  if(NOT GIT_TAGVER)
    message(FATAL_ERROR "No valid git tag found")
  else()
    string(REGEX MATCHALL "^v?([0-9]+)\.([0-9]+)\.([0-9]+)-r?([0-9]+)-(.*)$" _discard "${GIT_TAGVER}")
    set(${_var}_VERSION "" PARENT_SCOPE)

    if(CMAKE_MATCH_COUNT GREATER 1)
      set(${_var}_MAJOR ${CMAKE_MATCH_1} PARENT_SCOPE)
      set(${_var} "${CMAKE_MATCH_1}")
    endif()

    if(CMAKE_MATCH_COUNT GREATER 2)
      set(${_var}_MINOR ${CMAKE_MATCH_2} PARENT_SCOPE)
      set(${_var} "${${_var}}.${CMAKE_MATCH_2}")
    endif()

    if(CMAKE_MATCH_COUNT GREATER 3)
      set(${_var}_PATCH ${CMAKE_MATCH_3} PARENT_SCOPE)
      set(${_var} "${${_var}}.${CMAKE_MATCH_3}")
    endif()

    if(CMAKE_MATCH_COUNT GREATER 4)
      set(VERSION_TWEAK "${CMAKE_MATCH_4}")

      if(CMAKE_MATCH_COUNT GREATER 5)
        set(VERSION_TWEAK "${CMAKE_MATCH_5")
      endif()
      set(${_var}_TWEAK ${VERSION_TWEAK} PARENT_SCOPE)
      set(${_var} "${${_var}}-r${VERSION_TWEAK}")
    endif()

    set(${_var} "${${_var}}" PARENT_SCOPE)
    # set(${_var} "${${_var}_VERSION_MAJOR}.${${_var}_VERSION_MINOR}.${${_var}_VERSION_PATCH}" PARENT_SCOPE)
  endif()

endfunction()
