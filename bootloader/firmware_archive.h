#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cassert>

// Class for reading Firmware Archive (.fmi) files.
namespace FirmwareArchive
{
    class Block
    {
    public:
        uint32_t address;
        std::vector<uint8_t> data;
    };

    class Image
    {
    public:
        uint16_t usbVendorId;
        uint16_t usbProductId;
        uint16_t uploadType;
        std::vector<Block> blocks;
    };

    class Data
    {
    public:
        void readFromFile(std::istream & file, const char * fileName);

        operator bool() const
        {
            return !images.empty();
        }

        bool matchesBootloader(uint16_t usbVendorId, uint16_t usbProductId) const
        {
            for (const Image & image : images)
            {
                if (image.usbVendorId == usbVendorId &&
                    image.usbProductId == usbProductId)
                {
                    return true;
                }
            }
            return false;
        }

        const Image & findImage(uint16_t usbVendorId, uint16_t usbProductId) const
        {
            for (const Image & image : images)
            {
                if (image.usbVendorId == usbVendorId &&
                    image.usbProductId == usbProductId)
                {
                    return image;
                }
            }

            // This should never happen because we use matchesBootloader ahead
            // of time before calling this.
            assert(0);
            throw std::runtime_error("Matching image in firmware archive not found.");
        }

        std::string name;
        std::vector<Image> images;

    private:
        void processXml(std::string s);
    };
}

