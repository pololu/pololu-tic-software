#pragma once

#include <stdlib.h>

#include <cstdint>
#include <string>
#include <limits>

#include "exit_codes.h"
#include "exception_with_exit_code.h"

// Note: This will only work correctly for integer types T that have fewer than
// 64 bits.
template <typename T>
static T parse_arg_int(arg_reader & arg_reader, int base = 10)
{
  const char * value_c = arg_reader.next();
  if (value_c == NULL)
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "Expected a number after '" +
      std::string(arg_reader.last()) + "'.");
  }

  char * end;
  int64_t result = strtoll(value_c, &end, base);
  if (errno || *end)
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "The number after '" + std::string(arg_reader.last()) + "' is invalid.");
  }

  if (result < std::numeric_limits<T>::min())
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "The number after '" + std::string(arg_reader.last()) + "' is too small.");
  }

  if (result > std::numeric_limits<T>::max())
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "The number after '" + std::string(arg_reader.last()) + "' is too large.");
  }

  return result;
}

static inline std::string parse_arg_string(arg_reader & arg_reader)
{
    const char * value_c = arg_reader.next();
    if (value_c == NULL)
    {
      throw exception_with_exit_code(EXIT_BAD_ARGS,
        "Expected an argument after '" +
        std::string(arg_reader.last()) + "'.");
    }
    if (value_c[0] == 0)
    {
      throw exception_with_exit_code(EXIT_BAD_ARGS,
        "Expected a non-empty argument after '" +
        std::string(arg_reader.last()) + "'.");
    }
    return std::string(value_c);
}
