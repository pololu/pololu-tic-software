use_c99()

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# TODO: what about putting the major version in the shared library name?
# TODO: what about the RC file so people can easily see the version on Windows?

# Find libusbp using pkg-config and use it.
pkg_check_modules(LIBUSBP REQUIRED libusbp-1)

if (USE_SYSTEM_LIBYAML)
  # TODO: make sure using the systme libyaml actually works
  pkg_check_modules(YAML REQUIRED yaml)
  STRING(REPLACE ";" " " LIBYAML_LDFLAGS "${LIBYAML_LDFLAGS}")
else ()
  set (LIBYAML_LDFLAGS yaml)
  include_directories ("${CMAKE_SOURCE_DIR}/libyaml")
  set (LIBYAML_CFLAGS "-DYAML_DECLARE_STATIC")
endif ()

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBUSBP_CFLAGS} ${LIBYAML_CFLAGS}")

# Settings for GCC
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  # By default, symbols are not visible outside of the library.
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
endif ()

add_library (lib SHARED
  tic_baud_rate.c
  tic_current_limit.c
  tic_device.c
  tic_get_settings.c
  tic_set_settings.c
  tic_error.c
  tic_handle.c
  tic_names.c
  tic_settings.c
  tic_settings_read_from_string.c
  tic_settings_to_string.c
  tic_string.c
)

#include_directories (
#  "${CMAKE_CURRENT_SOURCE_DIR}"
#)

set_target_properties(lib PROPERTIES
  OUTPUT_NAME pololu-tic
  SOVERSION ${SOFTWARE_VERSION}
  VERSION ${SOFTWARE_VERSION}
  DEFINE_SYMBOL TIC_EXPORTS
)

target_link_libraries (lib "${LIBUSBP_LDFLAGS}" "${LIBYAML_LDFLAGS}")
