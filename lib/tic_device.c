#include "tic_internal.h"

struct tic_device
{
    libusbp_device usb_device;
    libusbp_generic_interface usb_interface;
    char serial_number[SERIAL_NUMBER_MAX_LENGTH + 1];
    uint16_t firmwareVersion;
};

tic_error * tic_list_connected_devices(
    tic_device *** device_list,
    size_t * device_count)
{
    tic_error * error = NULL;

    libusbp_device ** usb_device_list = NULL;
    size_t usb_device_count = 0;
    if (error == NULL)
    {
        error = tic_usb_error(
            libusbp_list_connected_devices(&device_list, &device_count));
    }

    

    // TODO: finish

    libusbp_list_free(usb_device_list);

    return NULL;
}

void tic_list_free(tic_device ** list);

tic_error * tic_device_copy(
    const tic_device * source,
    tic_device ** dest);

void tic_device_free(tic_device *);

const char * tic_device_get_name(const tic_device *);

const char * tic_device_get_serial_number(const tic_device *);

const char * tic_device_get_os_id(const tic_device *);
