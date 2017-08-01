#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cassert>

// Class for reading Firmware Archive (.fmi) files.
namespace firmware_archive
{
  class block
  {
  public:
    uint32_t address;
    std::vector<uint8_t> data;
  };

  class image
  {
  public:
    uint16_t usb_vendor_id;
    uint16_t usb_product_id;
    uint16_t upload_type;
    std::vector<block> blocks;
  };

  class data
  {
  public:
    void read_from_string(const std::string &);

    operator bool() const
    {
      return !images.empty();
    }

    bool matches_bootloader(uint16_t vendor_id, uint16_t product_id) const
    {
      for (const image & image : images)
      {
        if (image.usb_vendor_id == vendor_id &&
          image.usb_product_id == product_id)
        {
          return true;
        }
      }
      return false;
    }

    const image & find_image(uint16_t vendor_id, uint16_t product_id) const
    {
      for (const image & image : images)
      {
        if (image.usb_vendor_id == vendor_id &&
          image.usb_product_id == product_id)
        {
          return image;
        }
      }

      throw std::runtime_error("Matching image in firmware archive not found.");
    }

    std::string name;
    std::vector<image> images;

  private:
    void process_xml(const std::string &);
  };
}

