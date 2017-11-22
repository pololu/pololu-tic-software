#include "bootloader.h"

enum PloaderTypeIds
{
  ID_NONE = 0,
  ID_TIC_T825_BOOTLOADER,
  ID_TIC_T825_APP,
  ID_TIC_T834_BOOTLOADER,
  ID_TIC_T834_APP,
};

const std::vector<PloaderAppType> ploaderAppTypes = {
  {
    /* id */ ID_TIC_T825_APP,
    /* usbVendorId */ 0x1FFB,
    /* usbProductId */ 0x00B3,
    /* name */ "Tic T825 Stepper Motor Controller",
    /* composite */ false,
    /* interfaceNumber */ 0,
  },
  {
    /* id */ ID_TIC_T834_APP,
    /* usbVendorId */ 0x1FFB,
    /* usbProductId */ 0x00B5,
    /* name */ "Tic T834 Stepper Motor Controller",
    /* composite */ false,
    /* interfaceNumber */ 0,
  },
};

const std::vector<PloaderType> ploaderTypes = {
  {
    /* id */ ID_TIC_T825_BOOTLOADER,
    /* usbVendorId */ 0x1FFB,
    /* usbProductId */ 0x00B2,
    /* name */ "Tic T825 Bootloader",
    /* short_name */ "T825 bootloader",
    /* appAddress */ 0x2000,
    /* appSize */ 0x6000,
    /* writeBlockSize */ 0x40,
    /* erasingFlashAffectsEeprom */ true,
    /* supportsFlashPlainWriting */ false,
    /* supportsFlashReading */ false,
    /* eepromAddress */ 0,
    /* eepromAddressHexFile */ 0xF00000,
    /* eepromSize */ 0x100,
    /* supportsEepromAccess */ true,
    /* deviceCode */ NULL,
    /* matchingAppTypes */ { ID_TIC_T825_APP },
  },
  {
    /* id */ ID_TIC_T834_BOOTLOADER,
    /* usbVendorId */ 0x1FFB,
    /* usbProductId */ 0x00B4,
    /* name */ "Tic T834 Bootloader",
    /* short_name */ "T834 bootloader",
    /* appAddress */ 0x2000,
    /* appSize */ 0x6000,
    /* writeBlockSize */ 0x40,
    /* erasingFlashAffectsEeprom */ true,
    /* supportsFlashPlainWriting */ false,
    /* supportsFlashReading */ false,
    /* eepromAddress */ 0,
    /* eepromAddressHexFile */ 0xF00000,
    /* eepromSize */ 0x100,
    /* supportsEepromAccess */ true,
    /* deviceCode */ NULL,
    /* matchingAppTypes */ { ID_TIC_T834_APP },
  },
};

const std::vector<PloaderUserType> ploaderUserTypes = {
  {
    /* codeName */ "tic",
    /* name */ "Tic Stepper Motor Controller",
    /* memberIds */ { ID_TIC_T825_BOOTLOADER, ID_TIC_T825_APP,
                      ID_TIC_T825_BOOTLOADER, ID_TIC_T825_APP, },
  },
};
