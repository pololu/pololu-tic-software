use_c99()

# Find libusbp using pkg-config and use it.
pkg_check_modules(LIBUSBP REQUIRED libusbp-1)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBUSBP_CFLAGS}")

# Settings for GCC
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  # By default, symbols are not visible outside of the library.
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
endif ()

add_library (lib STATIC
  tic_error.c
  tic_device.c
  tic_handle.c
)

#include_directories (
#  "${CMAKE_CURRENT_SOURCE_DIR}"
#)

set_target_properties(lib PROPERTIES
  OUTPUT_NAME libpololu-tic
)

target_link_libraries (lib "${LIBUSBP_LDFLAGS}")
