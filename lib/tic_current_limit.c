#include "tic_internal.h"

uint32_t tic_get_max_allowed_current(uint8_t product)
{
  switch (product)
  {
  case TIC_PRODUCT_T834:
    return TIC_MAX_ALLOWED_CURRENT_T834;
  // TODO: case TIC_PRODUCT_T500:
  default:
    return TIC_MAX_ALLOWED_CURRENT;
  }
}

static uint8_t fix_current_limit_code(uint8_t product, uint8_t code)
{
  uint8_t max = tic_get_max_allowed_current(product)
    / TIC_CURRENT_LIMIT_UNITS_MA;

  if (code > max) { code = max; }
  else if (code > 64) { code &= ~3; }
  else if (code > 32) { code &= ~1; }
  return code;
}

// Converts the current limit code value as stored on the EEPROM to the actual
// current limit it represents, taking into account the limitations of the
// firmware.
uint32_t tic_current_limit_from_code(uint8_t product, uint8_t code)
{
  return fix_current_limit_code(product, code) * TIC_CURRENT_LIMIT_UNITS_MA;
}

// Converts a current limit in milliamps to a corresponding current limit code
// for the firmware, taking into account the limitations of the firmware.
// By design, this errs on the side of rounding down.
uint16_t tic_current_limit_to_code(uint8_t product, uint32_t current_limit)
{
  return fix_current_limit_code(product, current_limit / TIC_CURRENT_LIMIT_UNITS_MA);
}
