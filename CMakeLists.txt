cmake_minimum_required (VERSION 2.8.11)
project (tic)

set(POLOLU_BUILD FALSE CACHE BOOL
  "True if this software is being built by Pololu, and should use the Pololu logo.")

set(ENABLE_GUI TRUE CACHE BOOL
  "True if you want to build the GUI, which depends on Qt 5.")

set (CLI_NAME "ticcmd")
set (GUI_NAME "ticgui")
set (DOCUMENTATION_URL "https://www.pololu.com/")  # TODO: like to guide

set (SOFTWARE_VERSION_MAJOR 1)
set (SOFTWARE_VERSION_MINOR 0)
set (SOFTWARE_VERSION_PATCH 0)
set (SOFTWARE_VERSION ${SOFTWARE_VERSION_MAJOR}.${SOFTWARE_VERSION_MINOR}.${SOFTWARE_VERSION_PATCH})

string(TIMESTAMP YEAR "%Y")

find_package(PkgConfig)

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Options are Debug Release RelWithDebInfo MinSizeRel" FORCE)
endif ()

# Our C++ code uses features from the C++11 standard.
macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)

# Put libraries and executables in the top level of the build directory
# so that the executables can find the libraries and it is easy to run
# everything.
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Warn about everything.
set (CMAKE_C_FLAGS "-Wall -Wextra ${CMAKE_C_FLAGS}")
set (CMAKE_CXX_FLAGS "-Wall -Wextra ${CMAKE_CXX_FLAGS}")

if (WIN32)
  # Enable correct behavior for the return value of vsnprintf.
  add_definitions (-D__USE_MINGW_ANSI_STDIO=1)

  # Enable functions only available in Windows Vista and later.
  add_definitions (-D_WIN32_WINNT=0x0600 -DNTDDI_VERSION=0x06000000)
endif ()

# Detect Linux.
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set (LINUX 1)
endif ()

# Fix a warning on Mac OS X.
if (NOT CMAKE_VERSION VERSION_LESS "3.0")
  cmake_policy(SET CMP0042 NEW)
endif ()

# Find libusbp and use it.
pkg_check_modules(LIBUSBP REQUIRED libusbp-1)
set (CMAKE_CXX_FLAGS "${LIBUSBP_CFLAGS} ${CMAKE_CXX_FLAGS}")

include_directories (
  "${CMAKE_SOURCE_DIR}/include"
  "${CMAKE_BINARY_DIR}/include"
)

configure_file (
  "include/pavrpgm_config.h.in"
  "include/pavrpgm_config.h"
)

file(WRITE "${CMAKE_BINARY_DIR}/version.txt" "${SOFTWARE_VERSION}")

add_subdirectory (lib)
add_subdirectory (cli)

if (ENABLE_GUI)
  # TODO: add_subdirectory (gui)
endif ()

if (WIN32)
  # TODO: add_subdirectory (windows-installer)
endif ()

if (APPLE)
  # TODO: add_subdirectory (mac-installer)
endif ()
