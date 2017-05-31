#pragma once

#include <cstdint>
#include <string>
#include <limits>

static inline bool nice_str_to_num(std::string string, uint32_t & out, int base = 10)
{
  out = 0;
  try
  {
    size_t pos;
    out = stoul(string, &pos, base);
    return pos != string.size();
  }
  catch(const std::invalid_argument & e) { }
  catch(const std::out_of_range & e) { }
  return true;
}

static inline bool nice_str_to_num(std::string string, int32_t & out, int base = 10)
{
  out = 0;
  try
  {
    size_t pos;
    out = stol(string, &pos, base);
    return pos != string.size();
  }
  catch(const std::invalid_argument & e) { }
  catch(const std::out_of_range & e) { }
  return true;
}

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

  T value;
  if (nice_str_to_num(value_c, value, base))
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "The number after '" + std::string(arg_reader.last()) + "' is invalid.");
  }

  if (value > std::numeric_limits<T>::max())
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "The number after '" + std::string(arg_reader.last()) + "' is too large.");
  }

  return value;
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
