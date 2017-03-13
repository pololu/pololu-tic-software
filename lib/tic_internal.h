#pragma once

#include <tic.h>
#include <libusbp.h>
#include "tic_usb_protocol.h"

#define USB_VENDOR_ID_POLOLU 0x1FFB
#define USB_PRODUCT_ID_TIC01A 0x00B3

// A setup packet bRequest value from USB 2.0 Table 9-4
#define USB_REQUEST_GET_DESCRIPTOR 6

// A descriptor type from USB 2.0 Table 9-5
#define USB_DESCRIPTOR_TYPE_STRING 3

#define TIC_FIRMWARE_MODIFICATION_STRING_INDEX 4

const libusbp_generic_interface *
tic_device_get_generic_interface(cosnt tic_device * device);

// Error creation functions.
tic_error * tic_error_add_code(tic_error * error, uint32_t code);
tic_error * tic_error_add(tic_error * error, const char * format, ...);
tic_error * tic_error_create(const char * format, ...);
tic_error * tic_usb_error(libusbp_error *);
extern tic_error tic_error_no_memory;
