// Helpers for converting strings to integers.
//
// We don't use strtoll because there is some bug that causes it to to always
// return error 22 when run inside a Qt application on MacOS.

#pragma once

#include <stdint.h>
#include <type_traits>
#include <limits>

#define STRING_TO_INT_ERR_SMALL 1
#define STRING_TO_INT_ERR_LARGE 2
#define STRING_TO_INT_ERR_EMPTY 3
#define STRING_TO_INT_ERR_INVALID 4

// Converts a decimal string to the specified integer type. returning an error
// if there is non-number junk in the string or the number is out of range.
template <typename T>
static uint8_t string_to_int(const char * str, T * out)
{
  *out = 0;

  const char * p = str;

  // Process minus and plus signs.
  bool negative = false;
  if (*p == '-')
  {
    negative = true;
    p++;
  }
  if (*p == '+')
  {
    p++;
  }

  // Reject numbers with no digits.
  if (*p == 0)
  {
    return STRING_TO_INT_ERR_EMPTY;
  }

  T result = 0;
  while (*p)
  {
    uint8_t digit_value;
    if (*p >= '0' && *p <= '9')
    {
      digit_value = *p - '0';
    }
    else
    {
      // Expected a digit, got something else.
      return STRING_TO_INT_ERR_INVALID;
    }

    if (negative)
    {
      if (result < std::numeric_limits<T>::min() / 10)
      {
        return STRING_TO_INT_ERR_SMALL;  // Multiplication would underflow.
      }
      result *= 10;

      if (result < std::numeric_limits<T>::min() + digit_value)
      {
        return STRING_TO_INT_ERR_SMALL;  // Subtraction would underflow.
      }
      result -= digit_value;
    }
    else
    {
      if (result > std::numeric_limits<T>::max() / 10)
      {
        return STRING_TO_INT_ERR_LARGE;  // Multiplication would overflow.
      }
      result *= 10;

      if (result > std::numeric_limits<T>::max() - digit_value)
      {
        return STRING_TO_INT_ERR_LARGE;  // Addition would overflow.
      }
      result += digit_value;
    }

    p++;
  }

  *out = result;
  return 0;
}

// Converts an unprefixed hex string to the specified integer type, returning
// an error if there is non-number junk in the string or the number is out of
// range.
template <typename T>
static uint8_t hex_string_to_int(const char * str, T * out)
{
  *out = 0;

  const char * p = str;

  // Process minus and plus signs.
  bool negative = false;
  if (*p == '-')
  {
    negative = true;
    p++;
  }
  if (*p == '+')
  {
    p++;
  }

  // Reject numbers with no digits.
  if (*p == 0)
  {
    return STRING_TO_INT_ERR_EMPTY;
  }

  T result = 0;
  while (*p)
  {
    uint8_t digit_value;
    if (*p >= '0' && *p <= '9')
    {
      digit_value = *p - '0';
    }
    else if (*p >= 'a' && *p <= 'f')
    {
      digit_value = *p - 'a' + 10;
    }
    else if (*p >= 'A' && *p <= 'F')
    {
      digit_value = *p - 'A' + 10;
    }
    else
    {
      // Expected a digit, got something else.
      return STRING_TO_INT_ERR_INVALID;
    }

    if (negative)
    {
      if (result < std::numeric_limits<T>::min() / 0x10)
      {
        return STRING_TO_INT_ERR_SMALL;  // Multiplication would underflow.
      }
      result *= 0x10;

      if (result < std::numeric_limits<T>::min() + digit_value)
      {
        return STRING_TO_INT_ERR_SMALL;  // Subtraction would underflow.
      }
      result -= digit_value;
    }
    else
    {
      if (result > std::numeric_limits<T>::max() / 0x10)
      {
        return STRING_TO_INT_ERR_LARGE;  // Multiplication would overflow.
      }
      result *= 0x10;

      if (result > std::numeric_limits<T>::max() - digit_value)
      {
        return STRING_TO_INT_ERR_LARGE;  // Addition would overflow.
      }
      result += digit_value;
    }

    p++;
  }

  *out = result;
  return 0;
}
