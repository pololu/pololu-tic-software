use_cxx11()

pkg_check_modules(LIBUSBP REQUIRED libusbp-1)
string (REPLACE ";" " " LIBUSBP_CFLAGS "${LIBUSBP_CFLAGS}")
string (REPLACE ";" " " LIBUSBP_LDFLAGS "${LIBUSBP_LDFLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${LIBUSBP_CFLAGS} ${LIBYAML_CFLAGS}")

add_library (bootloader STATIC
  bootloader.cpp
  bootloader_data.cpp
  firmware_archive.cpp
  tinyxml2.cpp
)

set_property (TARGET bootloader PROPERTY
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})

set_property (TARGET bootloader PROPERTY
  INTERFACE_COMPILE_OPTIONS ${LIBUSBP_CFLAGS})

# target_link_libraries (bootloader "${LIBUSBP_LDFLAGS}")

