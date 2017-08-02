#pragma once

#include "firmware_archive.h"
#include <libusbp.hpp>
#include <vector>

typedef std::vector<uint8_t> MemoryImage;

#define UPLOAD_TYPE_STANDARD 0
#define UPLOAD_TYPE_DEVICE_SPECIFIC 1
#define UPLOAD_TYPE_PLAIN 2

enum MemorySet
{
    MEMORY_SET_ALL,
    MEMORY_SET_FLASH,
    MEMORY_SET_EEPROM,
};

/** Represents a type of USB device that can be used to start a bootloader.
 * Currently, just native USB interfaces are supported, but we could add more
 * fields in the future to support USB serial ports and USB HIDs. */
class PloaderAppType
{
public:
    uint32_t id;

    uint16_t usbVendorId, usbProductId;

    const char * name;

    bool composite;

    uint8_t interfaceNumber;

    bool operator ==(const PloaderAppType & other) const
    {
        return id == other.id;
    }
};

extern const std::vector<PloaderAppType> ploaderAppTypes;

/** Represents a specific device connected to the system
 * that we could use to start a bootloader. */
class PloaderAppInstance
{
public:
    PloaderAppType type;
    std::string serialNumber;

    PloaderAppInstance()
    {
    }

    PloaderAppInstance(const PloaderAppType type,
        libusbp::generic_interface gi,
        std::string serialNumber)
        : type(type), serialNumber(serialNumber), usbInterface(gi)
    {
    }

    operator bool() const
    {
        return usbInterface;
    }

    void launchBootloader();

private:
    libusbp::generic_interface usbInterface;
};

/** Represents a type of bootloader. */
class PloaderType
{
public:

    uint32_t id;

    uint16_t usbVendorId, usbProductId;

    /** A pointer to an ASCII string with the name of the bootloader.
     * Should be the same as the USB product string descriptor. */
    const char * name;

    const char * short_name;

    /* The address of the first byte of the app (used in the USB protocol). */
    uint32_t appAddress;

    /* The number of bytes in the app. */
    uint32_t appSize;

    /* The number of bytes you have to write at once while writing to flash. */
    uint16_t writeBlockSize;

    /* True if the erase action does something to the EEPROM. */
    bool erasingFlashAffectsEeprom;

    /* True if you can write plaintext data to flash. */
    bool supportsFlashPlainWriting;

    /* True if you can read from flash. */
    bool supportsFlashReading;

    /* The address of the first byte of EEPROM (used in the USB protocol). */
    uint32_t eepromAddress;

    /* The address used for the first byte of EEPROM in the HEX file. */
    uint32_t eepromAddressHexFile;

    /* The number of bytes in EEPROM.  Set this to a non-zero number for devices
     * that have EEPROM, even if the bootloader does not support accessing
     * EEPROM. */
    uint32_t eepromSize;

    bool supportsEepromAccess;

    const uint8_t * deviceCode;

    std::vector<uint32_t> matchingAppTypes;

    bool memorySetIncludesFlash(MemorySet ms) const;
    bool memorySetIncludesEeprom(MemorySet ms) const;

    /* Raises an exception if reading from the specified memories is not
     * allowed. */
    void ensureReading(MemorySet ms) const;

    /* Raises an exception if erasing the specified memories is not allowed. */
    void ensureErasing(MemorySet ms) const;

    /* Raises an exception if reading and writing from EEPROM is not allowed. */
    void ensureEepromAccess() const;

    /* Raises an exception if reading from flash is not allowed. */
    void ensureFlashReading() const;

    /* Raises an exception if writing plain data to flash is not allowed. */
    void ensureFlashPlainWriting() const;

    /* Returns a vector of the app types that correspond to this bootloader.
     * When trying to write to this bootloader, these are the apps that you
     * should consider restarting. */
    std::vector<PloaderAppType> getMatchingAppTypes() const;

    bool operator ==(const PloaderType & other) const
    {
        return id == other.id;
    }
};

extern const std::vector<PloaderType> ploaderTypes;

/** Represents a specific bootloader connected to the system
 * and ready to be used. */
class PloaderInstance
{
public:
  PloaderType type;
  std::string serialNumber;

  PloaderInstance()
  {
  }

PloaderInstance(const PloaderType type,
  libusbp::generic_interface gi,
  std::string serialNumber)
  : type(type), serialNumber(serialNumber), usbInterface(gi)
  {
  }

  operator bool()
  {
    return usbInterface;
  }

  std::string get_short_name() const
  {
    return type.short_name;
  }

  std::string get_serial_number() const
  {
    return serialNumber;
  }

  std::string get_os_id() const
  {
    return usbInterface.get_os_id();
  }

  uint16_t get_vendor_id() const
  {
    return type.usbVendorId;
  }

  uint16_t get_product_id() const
  {
    return type.usbProductId;
  }

  libusbp::generic_interface usbInterface;
};

/* Represents a high-level device type or device family that can be used in
 * communications with the user.  Each "user type" can correspond to multiple
 * specific app types and bootloader types.  */
class PloaderUserType  // TODO: remove
{
public:

    /* A lowercase name with no spaces that can be used for a command-line
     * argument. */
    const char * codeName;

    /* A friendly English name for the device type/family. */
    const char * name;

    /* IDs of PloaderAppType and PloaderBootloaderType objects
     * that belong to this type/family. */
    std::vector<uint32_t> memberIds;

    std::vector<PloaderType> getMatchingTypes() const;
    std::vector<PloaderAppType> getMatchingAppTypes() const;
};

extern const std::vector<PloaderUserType> ploaderUserTypes;

const PloaderAppType * ploaderAppTypeLookup(uint16_t usbVendorId, uint16_t usbProductId);

const PloaderType * ploaderTypeLookup(uint16_t usbVendorId, uint16_t usbProductId);

const PloaderUserType * ploaderUserTypeLookup(std::string codeName);

/** Detects all the known apps that are currently connected to the computer.  */
std::vector<PloaderAppInstance> ploaderListApps();

/** Detects all the known bootloaders that are currently connected to the
 * computer.  */
std::vector<PloaderInstance> ploaderListBootloaders();

class PloaderStatusListener
{
public:
    virtual void set_status(const char * status,
        uint32_t progress, uint32_t maxProgress) = 0;
};

class PloaderHandle
{
public:
    PloaderHandle(PloaderInstance);

    PloaderHandle() { }

    operator bool() const noexcept { return handle; }

    void close()
    {
        *this = PloaderHandle();
    }

    /** Sends a request to the bootloader to initialize a particular type of
     * upload.  This is usually required before erasing or writing to flash. */
    void initialize(uint16_t uploadType);

    /** Calls initialize with some upload type that is known to work.  This is
     * useful for users that don't care about the upload type because they just
     * want to erase the device. */
    void initialize();

    /** Erases the entire application flash region. */
    void eraseFlash();

    /** Writes an arbitrary image to flash.  Before doing this,
     * you will need to initialize and erase. */
    void writeFlash(const uint8_t * image);

    /** Takes care of all the details of reading an app image from the flash on
     * a device.  image must be a pointer to a memory block of the right size which
     * will be written to by this function.  This function takes care of getting the
     * Wixel in to bootloader mode (if needed) and reading the image. */
    void readFlash(uint8_t * image);

    /** Erases the EEPROM (sets to 0xFF). **/
    void eraseEeprom();

    /** Just like writeFlash, but for EEPROM instead */
    void writeEeprom(const uint8_t * image);

    /** Just like readFlash, but for EEPROM instead. */
    void readEeprom(uint8_t * image);

    /** Sends the Restart command, which causes the device device to reset.  This is
     * usually used to allow a newly-loaded application to start running. */
    void restartDevice();

    /** Asks the bootloader if the currently-loaded application is valid or not.
     * Returns true if the application is valid. */
    bool checkApplication();

    // Erases flash and performs any other steps needed to apply the firmware
    // image to the device
    void applyImage(const firmware_archive::image & image);

    PloaderType type;

    void setStatusListener(PloaderStatusListener * listener)
    {
        this->listener = listener;
    }

private:
    void writeFlashBlock(const uint32_t address, const uint8_t * data, size_t size);
    void writeEepromBlock(const uint32_t address, const uint8_t * data, size_t size);
    void eraseEepromFirstByte();

    void reportError(const libusbp::error & error, std::string context)
        __attribute__((noreturn));

    PloaderStatusListener * listener;

    libusbp::generic_handle handle;
};

// TODO: move everything above into this name space and make it use snake case
namespace bootloader
{
  std::vector<PloaderInstance> list_connected_devices();
}
