use_c99()

# TODO: what about putting the major version in the shared library name?
# TODO: what about the RC file so people can easily see the version on Windows?

# Find libusbp using pkg-config and use it.
pkg_check_modules(LIBUSBP REQUIRED libusbp-1)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBUSBP_CFLAGS}")

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
  tic_settings.c
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

target_link_libraries (lib "${LIBUSBP_LDFLAGS}")
