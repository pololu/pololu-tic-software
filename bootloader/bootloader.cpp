// Code for talking to the device's bootloader and doing firmware upgrades.

// TODO: fix code style of this file and related files

#include "bootloader.h"
#include <cstring>

// Request codes used to talk to the bootloader.
#define REQUEST_INITIALIZE         0x80
#define REQUEST_ERASE_FLASH        0x81
#define REQUEST_WRITE_FLASH_BLOCK  0x82
#define REQUEST_GET_LAST_ERROR     0x83
#define REQUEST_CHECK_APPLICATION  0x84
#define REQUEST_READ_FLASH         0x86
#define REQUEST_SET_DEVICE_CODE    0x87
#define REQUEST_READ_EEPROM        0x88
#define REQUEST_WRITE_EEPROM       0x89
#define REQUEST_RESTART            0xFE

// Request codes used to talk to a typical native USB app.
#define REQUEST_START_BOOTLOADER  0xFF

// Error codes returned by REQUEST_ERASE_FLASH and REQUEST_GET_LAST_ERROR.
#define PLOADER_ERROR_STATE                1
#define PLOADER_ERROR_LENGTH               2
#define PLOADER_ERROR_PROGRAMMING          3
#define PLOADER_ERROR_WRITE_PROTECTION     4
#define PLOADER_ERROR_VERIFICATION         5
#define PLOADER_ERROR_ADDRESS_RANGE        6
#define PLOADER_ERROR_ADDRESS_ORDER        7
#define PLOADER_ERROR_ADDRESS_ALIGNMENT    8
#define PLOADER_ERROR_WRITE                9
#define PLOADER_ERROR_EEPROM_VERIFICATION 10

// Other bootloader constants
#define DEVICE_CODE_SIZE           16

static std::string ploaderGetErrorDescription(uint8_t errorCode)
{
  switch (errorCode)
  {
  case 0: return "Success.";
  case PLOADER_ERROR_STATE: return "Device is not in the correct state.";
  case PLOADER_ERROR_LENGTH: return "Invalid data length.";
  case PLOADER_ERROR_PROGRAMMING: return "Programming error.";
  case PLOADER_ERROR_WRITE_PROTECTION: return "Write protection error.";
  case PLOADER_ERROR_VERIFICATION: return "Verification error.";
  case PLOADER_ERROR_ADDRESS_RANGE: return "Address is not in the correct range.";
  case PLOADER_ERROR_ADDRESS_ORDER: return "Address was not accessed in the correct order.";
  case PLOADER_ERROR_ADDRESS_ALIGNMENT: return "Address does not have the correct alignment.";
  case PLOADER_ERROR_WRITE: return "Write error.";
  case PLOADER_ERROR_EEPROM_VERIFICATION: return "EEPROM verification error.";
  default: return std::string("Unknown error code: ") + std::to_string(errorCode) + ".";
  }
}

const PloaderAppType * ploaderAppTypeLookup(uint16_t usbVendorId, uint16_t usbProductId)
{
  for (const PloaderAppType & t : ploaderAppTypes)
  {
    if (t.usbVendorId == usbVendorId && t.usbProductId == usbProductId)
    {
      return &t;
    }
  }
  return NULL;
}

const PloaderType * ploaderTypeLookup(uint16_t usbVendorId, uint16_t usbProductId)
{
  for (const PloaderType & t : ploaderTypes)
  {
    if (t.usbVendorId == usbVendorId && t.usbProductId == usbProductId)
    {
      return &t;
    }
  }
  return NULL;
}

const PloaderUserType * ploaderUserTypeLookup(std::string codeName)
{
  for (const PloaderUserType & t : ploaderUserTypes)
  {
    if (codeName == t.codeName)
    {
      return &t;
    }
  }
  return NULL;
}

template <typename T>
static std::vector<T> fetchByIds(
  std::vector<T> items, const std::vector<uint32_t> & ids)
{
  std::vector<T> r;

  for (uint32_t id : ids)
  {
    for (const T & item : items)
    {
      if (item.id == id)
      {
        r.push_back(item);
        break;
      }
    }
  }

  return r;
}

std::vector<PloaderAppType> PloaderType::getMatchingAppTypes() const
{
  return fetchByIds(ploaderAppTypes, matchingAppTypes);
}

std::vector<PloaderAppType> PloaderUserType::getMatchingAppTypes() const
{
  return fetchByIds(ploaderAppTypes, memberIds);
}

std::vector<PloaderType> PloaderUserType::getMatchingTypes() const
{
  return fetchByIds(ploaderTypes, memberIds);
}

bool PloaderType::memorySetIncludesFlash(MemorySet ms) const
{
  return ms == MEMORY_SET_FLASH || ms == MEMORY_SET_ALL;
}

bool PloaderType::memorySetIncludesEeprom(MemorySet ms) const
{
  return ms == MEMORY_SET_EEPROM ||
    (ms == MEMORY_SET_ALL && supportsEepromAccess);
}

void PloaderType::ensureReading(MemorySet ms) const
{
  if (this->memorySetIncludesFlash(ms))
  {
    ensureFlashReading();
  }

  if (this->memorySetIncludesEeprom(ms))
  {
    ensureEepromAccess();
  }
}

void PloaderType::ensureErasing(MemorySet ms) const
{
  if (ms == MEMORY_SET_ALL)
  {
    // All devices support a full erase of all memories.
  }
  else if (ms == MEMORY_SET_FLASH)
  {
    if (erasingFlashAffectsEeprom)
    {
      throw std::runtime_error(
        "This bootloader does not support erasing flash "
        "without affecting EEPROM.");
    }
  }
  else if (ms == MEMORY_SET_EEPROM)
  {
    ensureEepromAccess();
  }
  else
  {
    assert(0);
    throw std::runtime_error("Unrecognized memory set.");
  }
}

void PloaderType::ensureEepromAccess() const
{
  if (eepromSize == 0)
  {
    throw std::runtime_error("This device does not have EEPROM.");
  }

  if (!supportsEepromAccess)
  {
    throw std::runtime_error("This bootloader does not support accessing EEPROM.");
  }
}

void PloaderType::ensureFlashReading() const
{
  if (!supportsFlashReading)
  {
    throw std::runtime_error("This bootloader does not support reading flash memory.");
  }
}

void PloaderType::ensureFlashPlainWriting() const
{
  if (!supportsFlashPlainWriting)
  {
    throw std::runtime_error(
      "This bootloader is not compatible with writing plain data to flash.");
  }
}

std::vector<PloaderAppInstance> ploaderListApps()
{
  // Get a list of all connected USB devices.
  std::vector<libusbp::device> devices = libusbp::list_connected_devices();

  std::vector<PloaderAppInstance> list;

  for (const libusbp::device & device : devices)
  {
    // Filter out things that are not known apps.
    const PloaderAppType * type = ploaderAppTypeLookup(device.get_vendor_id(),
      device.get_product_id());
    if (!type) { continue; }

    // Get the generic interface object.
    libusbp::generic_interface usb_interface;
    try
    {
      usb_interface = libusbp::generic_interface(device,
        type->interfaceNumber, type->composite);
    }
    catch(const libusbp::error & error)
    {
      if (error.has_code(LIBUSBP_ERROR_NOT_READY))
      {
        // This interface is not ready to be used yet.
        // This is normal if it was recently enumerated.
        continue;
      }
      throw;
    }

    PloaderAppInstance instance(*type, usb_interface, device.get_serial_number());
    list.push_back(instance);
  }

  return list;
}

void PloaderAppInstance::launchBootloader()
{
  try
  {
    libusbp::generic_handle handle(usbInterface);
    handle.control_transfer(0x40, REQUEST_START_BOOTLOADER, 0, 0);
  }
  catch(const libusbp::error & error)
  {
    throw std::runtime_error(
      std::string("Failed to start bootloader.  ") + error.what());
  }
}

std::vector<PloaderInstance> ploaderListBootloaders()
{
  // Get a list of all connected USB devices.
  std::vector<libusbp::device> devices = libusbp::list_connected_devices();

  std::vector<PloaderInstance> list;

  for (const libusbp::device & device : devices)
  {
    // Filter out things that are not bootloaders.
    const PloaderType * type = ploaderTypeLookup(device.get_vendor_id(),
      device.get_product_id());
    if (!type) { continue; }

    // Get the generic interface object for interface 0.
    libusbp::generic_interface usb_interface;
    try
    {
      usb_interface = libusbp::generic_interface(device);
    }
    catch(const libusbp::error & error)
    {
      if (error.has_code(LIBUSBP_ERROR_NOT_READY))
      {
        // This interface is not ready to be used yet.
        // This is normal if it was recently enumerated.
        continue;
      }
      throw;
    }

    PloaderInstance instance(*type, usb_interface, device.get_serial_number());
    list.push_back(instance);
  }

  return list;
}

std::vector<PloaderInstance> bootloader::list_connected_devices()
{
  return ploaderListBootloaders();
}

PloaderHandle::PloaderHandle(PloaderInstance instance) : type(instance.type)
{
  handle = libusbp::generic_handle(instance.usbInterface);
}

// This can be called after a USB request for writing EEPROM or flash fails.  If
// appropriate, it attempts to make another request to get a more specific error
// code from the device, and then throws an error with that information in it.
// If anything goes wrong, it just throws the original USB error.
void PloaderHandle::reportError(const libusbp::error & error, std::string context)
{
  if (!error.has_code(LIBUSBP_ERROR_STALL))
  {
    // This is an unusual error that was not just caused by a STALL packet,
    // so don't attempt to do anything.  Maybe the device didn't even see
    // the original USB request so the value returned by REQUEST_GET_LAST_ERROR
    // would be meaningless.
    throw error;
  }

  uint8_t errorCode = 0;
  size_t transferred = 0;
  try
  {
    handle.control_transfer(0xC0, REQUEST_GET_LAST_ERROR, 0, 0,
      &errorCode, 1, &transferred);
  }
  catch(const libusbp::error & second_error)
  {
    throw error;
  }

  if (transferred != 1)
  {
    throw error;
  }

  std::string message = context + ": " + ploaderGetErrorDescription(errorCode);
  throw std::runtime_error(message);
}

static std::runtime_error transfer_length_error(std::string context,
  size_t expected, size_t actual)
{
  return std::runtime_error(
    std::string("Incorrect transfer length while ") + context + ": "
    "expected " + std::to_string(expected) + ", "
    "got " + std::to_string(actual) + ".");
}

void PloaderHandle::initialize(uint16_t uploadType)
{
  if (type.deviceCode != NULL)
  {
    // The device code might be stored in read-only memory, which can cause
    // WinUSB to give error code 0x3e6 when we try to send it over USB.
    // Copy it to the stack.
    uint8_t b[DEVICE_CODE_SIZE];
    memcpy(b, type.deviceCode, DEVICE_CODE_SIZE);

    try
    {
      handle.control_transfer(0x40, REQUEST_SET_DEVICE_CODE, 0, 0,
        (void *)b, DEVICE_CODE_SIZE);
    }
    catch(const libusbp::error & error)
    {
      throw std::runtime_error(
        std::string("Failed to send device code: ") +
        error.message());
    }
  }

  try
  {
    handle.control_transfer(0x40, REQUEST_INITIALIZE, uploadType, 0);
  }
  catch(const libusbp::error & error)
  {
    throw std::runtime_error(
      std::string("Failed to initialize bootloader: ") +
      error.message());
  }
}

void PloaderHandle::initialize()
{
  uint16_t defaultUploadType;
  if (type.supportsFlashPlainWriting)
  {
    defaultUploadType = UPLOAD_TYPE_PLAIN;
  }
  else
  {
    defaultUploadType = UPLOAD_TYPE_STANDARD;
  }

  initialize(defaultUploadType);
}

void PloaderHandle::eraseFlash()
{
  int maxProgress = 0;

  while (true)
  {
    uint8_t response[2];
    size_t transferred;
    handle.control_transfer(0xC0, REQUEST_ERASE_FLASH, 0, 0,
      &response, sizeof(response), &transferred);
    if (transferred != 2)
    {
      throw transfer_length_error("erasing flash", 2, transferred);
    }
    uint8_t errorCode = response[0];
    uint8_t progressLeft = response[1];
    if (errorCode)
    {
      throw std::runtime_error("Error erasing page: " +
        ploaderGetErrorDescription(errorCode) + ".");
    }

    if (maxProgress < progressLeft)
    {
      maxProgress = progressLeft + 1;
    }

    if (listener)
    {
      uint32_t progress = maxProgress - progressLeft;
      listener->set_status("Erasing flash...", progress, maxProgress);
    }

    if (progressLeft == 0)
    {
      return;
    }
  }
}

void PloaderHandle::writeFlashBlock(uint32_t address, const uint8_t * data, size_t size)
{
  size_t transferred;
  try
  {
    handle.control_transfer(0x40, REQUEST_WRITE_FLASH_BLOCK,
      address & 0xFFFF, address >> 16 & 0xFFFF,
      (uint8_t *)data, type.writeBlockSize, &transferred);
  }
  catch(const libusbp::error & error)
  {
    reportError(error, "Failed to write flash");
  }

  if (transferred != size)
  {
    throw transfer_length_error("writing flash",
      type.writeBlockSize, transferred);
  }
}

void PloaderHandle::writeFlash(const uint8_t * image)
{
  assert(image != NULL);

  type.ensureFlashPlainWriting();

  uint32_t address = type.appAddress + type.appSize;
  while (address > type.appAddress)
  {
    // Advance to the next block.
    address -= type.writeBlockSize;
    assert((address % type.writeBlockSize) == 0);
    const uint8_t * block = &image[address - type.appAddress];

    // If the block is empty, don't write to it.
    bool blockIsEmpty = true;
    for (uint32_t i = 0; i < type.writeBlockSize; i++)
    {
      if (block[i] != 0xFF)
      {
        blockIsEmpty = false;
        break;
      }
    }
    if (blockIsEmpty)
    {
      continue;
    }

    // Write the block to flash.
    writeFlashBlock(address, block, type.writeBlockSize);

    if (listener)
    {
      // These progress numbers aren't very good because they don't
      // account for how many blocks actually need to be written to flash.
      uint32_t progress = type.appSize - (address - type.appAddress);
      listener->set_status("Writing flash...", progress, type.appSize);
    }
  }
}

void PloaderHandle::readFlash(uint8_t * image)
{
  assert(image != NULL);
  type.ensureFlashReading();

  const uint32_t endAddress = type.appAddress + type.appSize;

  const size_t blockSize = 1024;
  uint32_t address = type.appAddress;
  while(address < endAddress)
  {
    assert(address + blockSize <= endAddress);

    size_t transferred;
    handle.control_transfer(0xC0, REQUEST_READ_FLASH,
      address & 0xFFFF, address >> 16 & 0xFFFF,
      &image[address - type.appAddress], blockSize, &transferred);
    if (transferred != blockSize)
    {
      throw transfer_length_error("reading flash", blockSize, transferred);
    }

    address += blockSize;

    if (listener)
    {
      listener->set_status("Reading flash...",
        address - type.appAddress, type.appSize);
    }
  }
}

void PloaderHandle::eraseEeprom()
{
  type.ensureEepromAccess();

  MemoryImage image(type.eepromSize, 0xFF);
  writeEeprom(&image[0]);
}

void PloaderHandle::writeEepromBlock(uint32_t address,
  const uint8_t * data, size_t size)
{
  type.ensureEepromAccess();

  size_t transferred;
  try
  {
    handle.control_transfer(0x40, REQUEST_WRITE_EEPROM,
      address & 0xFFFF, address >> 16 & 0xFFFF,
      (uint8_t *)data, size, &transferred);
  }
  catch(const libusbp::error & error)
  {
    reportError(error, "Failed to write EEPROM");
  }
  if (transferred != size)
  {
    throw transfer_length_error("writing EEPROM", size, transferred);
  }
}

void PloaderHandle::eraseEepromFirstByte()
{
  type.ensureEepromAccess();

  uint8_t blankByte = 0xFF;
  writeEepromBlock(0, &blankByte, 1);
}

void PloaderHandle::writeEeprom(const uint8_t * image)
{
  type.ensureEepromAccess();

  const uint32_t endAddress = type.eepromAddress + type.eepromSize;

  // Set the message to "Erasing EEPROM..." if the image happens to be all 0xFF.
  // This is less surprising for people who were not intentionally trying to
  // put anything in EEPROM using software that calls this function to erase it.
  const char * message = "Erasing EEPROM...";
  for (uint32_t i = 0; i < type.eepromSize; i++)
  {
    if (image[i] != 0xFF)
    {
      message = "Writing EEPROM...";
      break;
    }
  }

  const uint32_t blockSize = 32;
  uint32_t address = type.eepromAddress;
  while (address < endAddress)
  {
    assert(address + blockSize <= endAddress);

    writeEepromBlock(address, image + address, blockSize);
    address += blockSize;
    if (listener)
    {
      uint32_t progress = address - type.eepromAddress;
      listener->set_status(message, progress, type.eepromSize);
    }
  }
}

void PloaderHandle::readEeprom(uint8_t * image)
{
  type.ensureEepromAccess();

  const uint32_t endAddress = type.eepromAddress + type.eepromSize;

  uint32_t address = type.eepromAddress;
  while (address < endAddress)
  {
    const uint32_t blockSize = 32;
    assert(address + blockSize <= endAddress);

    size_t transferred;
    handle.control_transfer(0xC0, REQUEST_READ_EEPROM,
      address & 0xFFFF, address >> 16 & 0xFFFF,
      &image[address], blockSize, &transferred);
    if (transferred != blockSize)
    {
      throw transfer_length_error("reading EEPROM", blockSize, transferred);
    }

    address += blockSize;

    if (listener)
    {
      uint32_t progress = address - type.eepromAddress;
      listener->set_status("Reading EEPROM...", progress, endAddress);
    }
  }
}

void PloaderHandle::applyImage(const firmware_archive::image & image)
{
  initialize(image.upload_type);

  eraseFlash();

  if (type.supportsEepromAccess)
  {
    // We erase the first byte of EEPROM so that the firmware is able to
    // know it has been upgraded and not accidentally use invalid settings
    // from an older version of the firmware.
    eraseEepromFirstByte();
  }

  size_t progress = 0;
  for (const firmware_archive::block & block : image.blocks)
  {
    writeFlashBlock(block.address, &block.data[0], block.data.size());

    if (listener)
    {
      progress++;
      listener->set_status("Writing flash...", progress, image.blocks.size());
    }
  }
}

void PloaderHandle::restartDevice()
{
  const uint16_t durationMs = 100;
  try
  {
    handle.control_transfer(0x40, REQUEST_RESTART, durationMs, 0);
  }
  catch(const libusbp::error & error)
  {
    throw std::runtime_error(
      std::string("Failed to restart device.") + error.what());
  }
}

bool PloaderHandle::checkApplication()
{
  uint8_t response;
  size_t transferred;
  handle.control_transfer(0xC0, REQUEST_CHECK_APPLICATION, 0, 0,
    &response, 1, &transferred);
  if (transferred != 1)
  {
    throw transfer_length_error("checking application", 1, transferred);
  }
  return response;
}

