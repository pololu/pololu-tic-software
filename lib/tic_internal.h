#pragma once

#include <tic.h>
#include <config.h>

#include <libusbp.h>
#include <yaml.h>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef _MSC_VER
#define TIC_PRINTF(f, a)
#else
#define TIC_PRINTF(f, a) __attribute__((format (printf, f, a)))
#endif

#define USB_VENDOR_ID_POLOLU 0x1FFB
#define USB_PRODUCT_ID_TIC01A 0x00B3

// A setup packet bRequest value from USB 2.0 Table 9-4
#define USB_REQUEST_GET_DESCRIPTOR 6

// A descriptor type from USB 2.0 Table 9-5
#define USB_DESCRIPTOR_TYPE_STRING 3

#define TIC_FIRMWARE_MODIFICATION_STRING_INDEX 4


// Internal string manipulation library.

typedef struct tic_string
{
  char * data;
  size_t capacity;
  size_t length;
} tic_string;

void tic_string_setup(tic_string *);
void tic_string_setup_dummy(tic_string *);
TIC_PRINTF(2, 3)
void tic_sprintf(tic_string *, const char * format, ...);
bool tic_string_to_i64(const char *, int64_t *);


// Internal name lookup library.

typedef struct tic_name
{
  const char * name;
  uint32_t code;
} tic_name;

bool tic_name_to_code(const tic_name * table, const char * name, uint32_t * code);
bool tic_code_to_name(const tic_name * table, uint32_t code, const char ** name);

extern const tic_name tic_bool_names[];
extern const tic_name tic_product_names[];
extern const tic_name tic_step_mode_names[];
extern const tic_name tic_control_mode_names[];
extern const tic_name tic_scaling_degree_names[];
extern const tic_name tic_decay_mode_names[];
extern const tic_name tic_pin_config_names[];


// Intenral variables functions.

void tic_variables_set_from_device(tic_variables *, const uint8_t * buffer);


// Internal settings conversion functions.

uint32_t tic_baud_rate_from_brg(uint16_t brg);
uint16_t tic_baud_rate_to_brg(uint32_t baud_rate);
uint32_t tic_current_limit_from_code(uint8_t code);
uint16_t tic_current_limit_to_code(uint32_t current_limit);


// Internal tic_device functions.

const libusbp_generic_interface *
tic_device_get_generic_interface(const tic_device * device);


// Internal tic_handle functions.

tic_error * tic_set_setting_byte(tic_handle * handle,
  uint8_t address, uint8_t byte);

tic_error * tic_get_setting_segment(tic_handle * handle,
  uint8_t address, size_t length, uint8_t * output);

tic_error * tic_get_variable_segment(tic_handle *handle,
  bool clear_errors_occurred, size_t index, size_t length, uint8_t * buf);


// Error creation functions.

tic_error * tic_error_add_code(tic_error * error, uint32_t code);

TIC_PRINTF(2, 3)
tic_error * tic_error_add(tic_error * error, const char * format, ...);

TIC_PRINTF(1, 2)
tic_error * tic_error_create(const char * format, ...);

tic_error * tic_usb_error(libusbp_error *);

extern tic_error tic_error_no_memory;


// Static helper functions

static inline uint32_t read_u32(const uint8_t * p)
{
  return p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
}

static inline int32_t read_i32(const uint8_t * p)
{
  return (int32_t)read_u32(p);
}

static inline uint32_t read_u16(const uint8_t * p)
{
  return p[0] + (p[1] << 8);
}
