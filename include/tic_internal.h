#pragma once

#include <tic.h>
#include <libusbp.h>

tic_error * tic_error_add_code(tic_error * error, uint32_t code);
tic_error * tic_error_add(tic_error * error, const char * format, ...);
tic_error * tic_error_create(const char * format, ...);
tic_error * tic_usb_error(libusbp_error *);
