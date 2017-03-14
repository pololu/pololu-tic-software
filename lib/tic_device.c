#include "tic_internal.h"

// TODO: handle LIBUSBP_ERROR_NOT_READY errors properly by ignoring the device that generated them

struct tic_device
{
    libusbp_generic_interface * usb_interface;
    char * serial_number;
    char * os_id;
    uint16_t firmware_version;
};

tic_error * tic_list_connected_devices(
    tic_device *** device_list,
    size_t * device_count)
{
    if (device_count) { *device_count = 0; }

    if (device_list == NULL)
    {
        return tic_error_create("Device list output pointer is null.");
    }

    *device_list = NULL;

    tic_error * error = NULL;

    libusbp_device ** usb_device_list = NULL;
    size_t usb_device_count = 0;
    if (error == NULL)
    {
        error = tic_usb_error(
            libusbp_list_connected_devices(&usb_device_list, &usb_device_count));
    }

    tic_device ** tic_device_list = NULL;
    size_t tic_device_count = 0;
    if (error == NULL)
    {
        // Allocate enough memory for the case where every USB device is
        // relevant, without forgetting the NULL terminator.
        tic_device_list = calloc(usb_device_count + 1, sizeof(tic_device *));
        if (tic_device_list == NULL)
        {
            error = &tic_error_no_memory;
        }
    }

    for (size_t i = 0; error != NULL && i < usb_device_count; i++)
    {
        libusbp_device * usb_device = usb_device_list[i];

        // Check the USB vendor ID.
        uint16_t vendor_id;
        error = tic_usb_error(libusbp_device_get_vendor_id(usb_device, &vendor_id));
        if (error) { break; }
        if (vendor_id != USB_VENDOR_ID_POLOLU) { continue; }

        // Check the USB product ID.
        uint16_t product_id;
        error = tic_usb_error(libusbp_device_get_vendor_id(usb_device, &product_id));
        if (error) { break; }
        if (product_id != USB_PRODUCT_ID_TIC01A) { continue; }

        // Get the USB interface.
        libusbp_generic_interface * usb_interface = NULL;
        {
            uint8_t interface_number = 0;
            bool composite = false;
            libusbp_error * usb_error = libusbp_generic_interface_create(
                usb_device, interface_number, composite, &usb_interface);
            if (usb_error)
            {
                if (libusbp_error_has_code(usb_error, LIBUSBP_ERROR_NOT_READY))
                {
                    // An error occurred that is normal if the interface is simply
                    // not ready to use yet.  Silently ignore this device.
                    continue;
                }
                error = tic_usb_error(usb_error);
                break;
            }
        }

        // Allocate the new device.
        tic_device * tic_device = calloc(1, sizeof(tic_device));
        if (tic_device == NULL)
        {
            error = &tic_error_no_memory;
            break;
        }
        tic_device_list[tic_device_count++] = tic_device;

        // Store the USB interface.  Must do this here so that it will get freed
        // if any of the calls below fail.
        tic_device->usb_interface = usb_interface;

        // Get the serial number.
        error = tic_usb_error(libusbp_device_get_serial_number(
            usb_device, &tic_device->serial_number));
        if (error) { break; }

        // Get the OS ID.
        error = tic_usb_error(libusbp_device_get_os_id(
            usb_device, &tic_device->os_id));
        if (error) { break; }

        // Get the firmware version.
        error = tic_usb_error(libusbp_device_get_revision(
            usb_device, &tic_device->firmware_version));
        if (error) { break; }
    }

    if (error == NULL)
    {
        // Success.  Give the list to the caller.
        *device_list = tic_device_list;
        if (device_count) { *device_count = tic_device_count; }
        tic_device_list = NULL;
        tic_device_count = 0;
    }

    for (size_t i = 0; i < tic_device_count; i++)
    {
        tic_device_free(tic_device_list[i]);
    }

    tic_list_free(tic_device_list);

    for (size_t i = 0; i < usb_device_count; i++)
    {
        libusbp_device_free(usb_device_list[i]);
    }

    libusbp_list_free(usb_device_list);

    return error;
}

void tic_list_free(tic_device ** list)
{
    free(list);
}

tic_error * tic_device_copy(
    const tic_device * source,
    tic_device ** dest)
{
    if (dest == NULL)
    {
        return tic_error_create("Device output pointer is null.");
    }

    *dest = NULL;

    if (source == NULL)
    {
        return NULL;
    }

    tic_error * error = NULL;

    return error;
}

void tic_device_free(tic_device * device)
{
    if (device != NULL)
    {
        libusbp_generic_interface_free(device->usb_interface);
        libusbp_string_free(device->os_id);
        libusbp_string_free(device->serial_number);
        free(device);
    }
}

const char * tic_device_get_name(const tic_device * device)
{
    if (device == NULL) { return ""; }
    return "Tic Stepper Motor Controller";  // TODO: use full product names
}

const char * tic_device_get_serial_number(const tic_device * device)
{
    if (device == NULL) { return ""; }
    return device->serial_number;
}

const char * tic_device_get_os_id(const tic_device * device)
{
    if (device == NULL) { return ""; }
    return device->os_id;
}

uint16_t tic_device_get_firmware_version(const tic_device * device)
{
    if (device == NULL) { return 0xFFFF; }
    return device->firmware_version;
}

const libusbp_generic_interface *
tic_device_get_generic_interface(const tic_device * device)
{
    if (device == NULL) { return NULL; }
    return device->usb_interface;
}
