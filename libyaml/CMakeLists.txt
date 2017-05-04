# Build libyaml as a static library.

use_c99()

add_definitions (-DYAML_DECLARE_STATIC)

add_library (yaml STATIC yaml.c)

include_directories (
  "${CMAKE_CURRENT_SOURCE_DIR}"
)
