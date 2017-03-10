use_cxx11()

add_library (lib STATIC
  programmer.cpp
  isp_freq_table.cpp
)

include_directories (
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

set_target_properties(lib PROPERTIES
  OUTPUT_NAME pavr2
)

target_link_libraries (lib "${LIBUSBP_LDFLAGS}")
