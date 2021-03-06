use_cxx11()

pkg_check_modules(LIBUSBP REQUIRED libusbp-1)
string (REPLACE ";" " " LIBUSBP_CFLAGS_STR "${LIBUSBP_CFLAGS}")
string (REPLACE ";" " " LIBUSBP_LDFLAGS_STR "${LIBUSBP_LDFLAGS}")

if (USE_SYSTEM_LIBTINYXML2)
  pkg_check_modules(LIBTINXML2 REQUIRED tinyxml2)
  STRING(REPLACE ";" " " LIBTINYXML2_CFLAGS_STR "${LIBTINYXML2_CFLAGS}")
  STRING(REPLACE ";" " " LIBTINYXML2_LDFLAGS_STR "${LIBTINYXML2_LDFLAGS}")
else ()
  set (LIBTINYXML2_SRC libtinyxml2/tinyxml2.cpp)
  include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/libtinyxml2")
endif ()

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${LIBUSBP_CFLAGS_STR} ${LIBTINYXML2_CFLAGS_STR}")

add_library (bootloader STATIC
  bootloader.cpp
  bootloader_data.cpp
  firmware_archive.cpp
  ${LIBTINYXML2_SRC}
)

set_property (TARGET bootloader PROPERTY
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})

set_property (TARGET bootloader PROPERTY
  INTERFACE_COMPILE_OPTIONS ${LIBUSBP_CFLAGS})

target_link_libraries (bootloader "${LIBUSBP_LDFLAGS_STR}")

