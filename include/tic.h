// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file tic.h
///
/// This file provides the C API for libpololu-tic, a library supports
/// communciating with the Pololu Tic USB Stepper Motor Controller.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// The maximum firmware major version supported by this library.
#define TIC_FIRMWARE_VERSION_MAJOR_MAX 1

#ifdef _MSC_VER
#define TIC_WARN_UNUSED _Check_return_
#else
#define TIC_WARN_UNUSED __attribute__((warn_unused_result))
#endif

#define TIC_API


// tic_error ////////////////////////////////////////////////////////////////////

/// A ::tic_error object represents an error that occurred in the library.  Many
/// functions return a tic_error pointer.  The convention is that a NULL pointer
/// indicates success.  If the pointer is not NULL, the caller can use the
/// pointer to get information about the error, and then must free the error at
/// some point by calling tic_error_free.
typedef struct tic_error tic_error;

/// Each ::tic_error can have 0 or more error codes that give additional
/// information about the error that might help the caller take the right action
/// when the error occurs.  This enum defines which error codes are possible.
enum tic_error_code
{
    /// There were problems allocating memory.  A memory shortage might be the
    /// root cause of the error, or there might be another error that is masked
    /// by the memory problems.
    TIC_ERROR_MEMORY = 1,

    /// Access was denied.  A common cause of this error on Windows is that
    /// another application has a handle open to the same device.
    TIC_ERROR_ACCESS_DENIED = 2,

    /// The device took too long to respond to a request or transfer data.
    TIC_ERROR_TIMEOUT = 3,

    /// The error might have been caused by the device being disconnected, but
    /// it is possible it was caused by something else.
    TIC_ERROR_DEVICE_DISCONNECTED = 4,
};

/// Attempts to copy an error.  If you copy a NULL ::tic_error
/// pointer, the result will also be NULL.  If you copy a non-NULL ::tic_error
/// pointer, the result will be non-NULL, but if there are issues allocating
/// memory, then the copied error might have different properties than the
/// original error, and it will have the ::TIC_ERROR_MEMORY code.
///
/// It is the caller's responsibility to free the copied error.
TIC_API TIC_WARN_UNUSED
tic_error * tic_error_copy(const tic_error *);

/// Frees a returned error object.  Passing the NULL pointer to this function is
/// OK.  Do not free the same non-NULL error twice.
TIC_API
void tic_error_free(tic_error *);

/// Returns true if the error has specified error code.  The error codes are
/// listed in the ::tic_error_code enum.
TIC_API TIC_WARN_UNUSED
bool tic_error_has_code(const tic_error *, uint32_t code);

/// Returns an English-language ASCII-encoded string describing the error.  The
/// message consists of one or more sentences.
///
/// The returned pointer will be valid at least until the error is freed.
TIC_API TIC_WARN_UNUSED
const char * tic_error_get_message(const tic_error *);


// tic_device ///////////////////////////////////////////////////////////////////

/// Represents a Tic that is or was connected to the computer.
///
typedef struct tic_device tic_device;

/// Finds all the Tic devices connected to the computer via USB and returns a
/// list of them.
///
/// The list is terminated by a NULL pointer.  The optional @a device_count
/// parameter is used to return the number of devices in the list.
///
/// If this function is successful (the returned error pointer is NULL), then
/// you must later free each device by calling tic_device_free() and free the
/// list by calling tic_list_free().  The order in which the list and the
/// retrieved objects are freed does not matter.
TIC_API TIC_WARN_UNUSED
tic_error * tic_list_connected_devices(
    tic_device *** device_list,
    size_t * device_count);

/// Frees a device list returned by ::tic_list_connected_devices.
///
TIC_API
void tic_list_free(tic_device ** list);

/// Makes a copy of a device object.  If this function is successful, you will
/// need to free the copy by calling tic_device_free() at some point.
TIC_API TIC_WARN_UNUSED
tic_error * tic_device_copy(
    const tic_device * source,
    tic_device ** dest);

/// Frees a device object.  Passing a NULL pointer to this function is OK.  Do
/// not free the same non-NULL pointer twice.
TIC_API
void tic_device_free(tic_device *);

/// Gets the name of the device as an ASCII-encoded string.
/// The string will be valid at least as long as the device object.
/// If the device is NULL, returns an empty string.
TIC_API TIC_WARN_UNUSED
const char * tic_device_get_name(const tic_device *);

/// Gets the serial number of the device as an ASCII-encoded string.
/// The string will be valid at least as long as the device object.
/// If the device is NULL, returns an empty string.
TIC_API TIC_WARN_UNUSED
const char * tic_device_get_serial_number(const tic_device *);

/// Get an operating system-specific identifier for this device as an
/// ASCII-encoded string.  The string will be valid at least as long as the
/// device object.
/// If the device is NULL, returns an empty string.
TIC_API TIC_WARN_UNUSED
const char * tic_device_get_os_id(const tic_device *);

/// Gets the firmware version as a binary-coded decimal number (e.g. 0x0100
/// means "1.00").  We recommend using tic_get_firmware_version_string() instead
/// of this function if possible.
TIC_API TIC_WARN_UNUSED
uint16_t tic_device_get_firmware_version(const tic_device *);


// tic_handle ///////////////////////////////////////////////////////////////////

/// Represents a handle to a Tic device.  The handle can be used to read and
/// write data from the device.
typedef struct tic_handle tic_handle;

/// Opens a handle to the specified device.  The handle must later be closed
/// with tic_handle_close().
///
/// On Windows, if another applications has a handle open already, then this
/// function will fail and the returned error will have code
/// ::TIC_ERROR_ACCESS_DENIED.
TIC_API TIC_WARN_UNUSED
tic_error * tic_handle_open(const tic_device *, tic_handle **);

/// Closes and frees the specified handle.  It is OK to pass NULL to this
/// function.  Do not close the same non-NULL handle twice.
TIC_API
void tic_handle_close(tic_handle *);

/// Gets the device object that corresponds to this handle.
/// The device object will be valid for at least as long as the handle.
TIC_API TIC_WARN_UNUSED
const tic_device * tic_handle_get_device(const tic_handle *);

/// Gets the firmware version string, including any special modification codes
/// (e.g. "1.07nc").  The string will be valid for at least as long as the device.
///
/// This function might need to do I/O, but it will cache the result so it is OK
/// to call it frequently.
TIC_API TIC_WARN_UNUSED
const char * tic_get_firmware_version_string(tic_handle *);

TIC_API TIC_WARN_UNUSED
tic_error * tic_get_debug_data(tic_handle *, uint8_t * data, size_t * size);

#ifdef __cplusplus
}
#endif
