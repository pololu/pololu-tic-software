#include "bootloader.h"

enum bootloader_type_ids
{
  ID_NONE = 0,
  ID_TIC_T825_BOOTLOADER,
  ID_TIC_T834_BOOTLOADER,
  ID_TIC_T500_BOOTLOADER,
};

const std::vector<bootloader_type> bootloader_types = {
  {
    /* id */ ID_TIC_T825_BOOTLOADER,
    /* usb_vendor_id */ 0x1FFB,
    /* usb_product_id */ 0x00B2,
    /* name */ "Tic T825 Bootloader",
    /* short_name */ "T825",
    /* app_address */ 0x2000,
    /* app_size */ 0x6000,
    /* write_block_size */ 0x40,
    /* eeprom_address */ 0,
    /* eeprom_address_hex_file */ 0xF00000,
    /* eeprom_size */ 0x100,
  },
  {
    /* id */ ID_TIC_T834_BOOTLOADER,
    /* usb_vendor_id */ 0x1FFB,
    /* usb_product_id */ 0x00B4,
    /* name */ "Tic T834 Bootloader",
    /* short_name */ "T834",
    /* app_address */ 0x2000,
    /* app_size */ 0x6000,
    /* write_block_size */ 0x40,
    /* eeprom_address */ 0,
    /* eeprom_address_hex_file */ 0xF00000,
    /* eeprom_size */ 0x100,
  },
  {
    /* id */ ID_TIC_T500_BOOTLOADER,
    /* usb_vendor_id */ 0x1FFB,
    /* usb_product_id */ 0x00BC,
    /* name */ "Tic T500 Bootloader",
    /* short_name */ "T500",
    /* app_address */ 0x2000,
    /* app_size */ 0x6000,
    /* write_block_size */ 0x40,
    /* eeprom_address */ 0,
    /* eeprom_address_hex_file */ 0xF00000,
    /* eeprom_size */ 0x100,
  },
};
