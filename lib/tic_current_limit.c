#include "tic_internal.h"

static uint8_t fix_current_limit_code(uint8_t code)
{
  if (code > 124) { code = 124; }
  else if (code > 64) { code &= ~3; }
  else if (code > 32) { code &= ~1; }
  return code;
}

// Converts the current limit code value as stored on the EEPROM to the actual
// current limit it represents, taking into account the limitations of the
// firmware.
uint32_t tic_current_limit_from_code(uint8_t code)
{
  return fix_current_limit_code(code) * TIC_CURRENT_LIMIT_UNITS_MA;
}

// Converts a current limit in milliamps to a corresponding current limit code
// for the firmware, taking into account the limitations of the firmware.
// By design, this errs on the side of rounding down.
uint16_t tic_current_limit_to_code(uint32_t current_limit)
{
  return fix_current_limit_code(current_limit / TIC_CURRENT_LIMIT_UNITS_MA);
}

