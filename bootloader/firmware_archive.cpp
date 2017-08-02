#include "bootloader.h"
#include "tinyxml2.h"
#include <limits>
#include <sstream>

#define USB_VENDOR_ID_POLOLU 0x1FFB

static int hex_digit_value(uint8_t c)
{
  if (c >= 'a' && c <= 'f') { return c - 'a' + 10; }
  if (c >= 'A' && c <= 'F') { return c - 'A' + 10; }
  if (c >= '0' && c <= '9') { return c - '0'; }
  return -1;
}

static std::vector<std::string> split(const std::string & str, char delimiter)
{
  std::vector<std::string> r;
  std::stringstream stream(str);
  std::string item;
  while (std::getline(stream, item, delimiter))
  {
    r.push_back(item);
  }
  return r;
}

// Converts a string to an int64_t, returning an error if there is non-number
// junk in the string or the number is out of range.
static bool hex_string_to_i64(const char * str, int64_t * out)
{
  assert(str != NULL);
  assert(out != NULL);
  assert(sizeof(long long) == 8);

  *out = 0;

  char * end;
  int64_t result = strtoll(str, &end, 16);
  if (errno)
  {
    return false;
  }
  if (*end != 0)
  {
    // Non-number junk in the string.
    return false;
  }

  *out = result;
  return true;
}

static firmware_archive::block process_xml_block(
  const tinyxml2::XMLElement * element)
{
  assert(element != NULL);

  firmware_archive::block block;

  // Get the address.
  const char * address_c_str = element->Attribute("address");
  if (address_c_str == NULL)
  {
    throw std::runtime_error("A block is missing an address.");
  }

  int64_t address;
  if (!hex_string_to_i64(address_c_str, &address))
  {
    throw std::runtime_error("A block has an invalid address.");
  }
  if (address < 0 || address > std::numeric_limits<typeof(block.address)>::max())
  {
    throw std::runtime_error("A block has an out of range address.");
  }
  block.address = address;

  // Get the contents.
  const char * contents_c_str = element->GetText();
  if (contents_c_str == NULL)
  {
    throw std::runtime_error("A block has missing or invalid contents.");
  }

  std::string contents(contents_c_str);

  if ((contents.size() % 2) != 0)
  {
    throw std::runtime_error("A block has an odd number of characters.");
  }

  uint32_t byte_count = contents.size() / 2;

  std::istringstream stream(contents);
  block.data.resize(byte_count);

  for (uint32_t i = 0; i < byte_count; i++)
  {
    int v1 = hex_digit_value(contents[i * 2 + 0]);
    int v2 = hex_digit_value(contents[i * 2 + 1]);

    if (v1 < 0 || v2 < 0)
    {
      throw std::runtime_error("Invalid hex digit.");
    }

    block.data[i] = v1 * 16 + v2;
  }

  return block;
}

static firmware_archive::image process_xml_firmware_image(
  const tinyxml2::XMLElement * element)
{
  assert(element != NULL);

  firmware_archive::image image;

  // Get the product ID.
  const char * product_c_str = element->Attribute("product");
  if (product_c_str == NULL)
  {
    throw std::runtime_error("A firmware image is missing a product ID.");
  }
  int64_t product_id;
  if (!hex_string_to_i64(product_c_str, &product_id))
  {
    throw std::runtime_error("A firmware image has an invalid product ID.");
  }
  if (product_id < 0 || product_id > 0xffff)
  {
    throw std::runtime_error("A firmware image has an out of range product ID.");
  }
  image.usb_product_id = product_id;

  // FMI files don't store the vendor ID so we assume they are for Pololu
  // products.
  image.usb_vendor_id = USB_VENDOR_ID_POLOLU;

  // Get the upload type.
  image.upload_type = UPLOAD_TYPE_STANDARD;
  const char * upload_type_c_str = element->Attribute("uploadType");
  if (upload_type_c_str)
  {
    std::string upload_type = upload_type_c_str;
    if (upload_type == "Standard")
    {
      image.upload_type = UPLOAD_TYPE_STANDARD;
    }
    else if (upload_type == "DeviceSpecific")
    {
      image.upload_type = UPLOAD_TYPE_DEVICE_SPECIFIC;
    }
    else if (upload_type == "Plain")
    {
      image.upload_type = UPLOAD_TYPE_PLAIN;
    }
    else
    {
      throw std::runtime_error("Invalid upload type specified in file.");
    }
  }

  // Process the blocks.
  for (const tinyxml2::XMLNode * node = element->FirstChild();
       node != NULL; node = node->NextSibling())
  {
    const tinyxml2::XMLElement * element = node->ToElement();
    if (element == NULL)
    {
      continue;
    }

    if (std::string(element->Name()) != "Block")
    {
      continue;
    }

    image.blocks.push_back(process_xml_block(element));
  }

  if (image.blocks.empty())
  {
    throw std::runtime_error("An image has no blocks in it.");
  }

  return image;
}

void firmware_archive::data::process_xml(const std::string & string)
{
  tinyxml2::XMLDocument doc;
  doc.Parse(string.c_str(), string.size());
  if (doc.Error())
  {
    throw std::runtime_error(std::string("XML error: ") + doc.ErrorName() + ".");
  }

  // Check the root element.
  tinyxml2::XMLElement * root = doc.RootElement();
  if (root == NULL)
  {
    throw std::runtime_error("The firmware archive has no root element.");
  }
  if (std::string(root->Name()) != "FirmwareArchive")
  {
    throw std::runtime_error("The firmware archive root element has an invalid name.");
  }

  // Check the format attribute.
  std::string format = root->Attribute("format");
  std::vector<std::string> parts = split(format, '.');
  if (parts[0] != "1")
  {
    throw std::runtime_error(
      "The firmware archive format is different than expected.  "
      "Try installing the latest version of this software.");
  }

  // Get the name attribute.
  const char * name = root->Attribute("name");
  if (name != NULL) { this->name = name; }

  // Process the images.
  for (const tinyxml2::XMLNode * node = root->FirstChild();
       node != NULL; node = node->NextSibling())
  {
    const tinyxml2::XMLElement * element = node->ToElement();
    if (element == NULL)
    {
      continue;
    }

    if (std::string(element->Name()) != "FirmwareImage")
    {
      continue;
    }

    images.push_back(process_xml_firmware_image(element));
  }

  if (images.empty())
  {
    throw std::runtime_error("The firmware archive has no images.");
  }
}

void firmware_archive::data::read_from_string(const std::string & string)
{
  name.clear();
  images.clear();
  try
  {
    process_xml(string);
  }
  catch (const std::runtime_error & e)
  {
    throw std::runtime_error(
      std::string("There was an error processing the firmware archive.  ")
      + e.what());
  }
}

// This is just for debugging.
std::string firmware_archive::data::dump_string() const
{
  std::stringstream r;
  r << std::hex;
  r << "Name: " << name << std::endl;
  r << "Images: 0x" << images.size() << std::endl;
  for (const auto & image : images)
  {
    r << "Image for " << image.usb_vendor_id << ":" << image.usb_product_id
      << "," << image.upload_type << std::endl;
    r << "Blocks: 0x" << image.blocks.size() << std::endl;
    for (uint32_t i = 0; i < 3 && i < image.blocks.size(); i++)
    {
      const auto & block = image.blocks[i];
      r << "Block address: 0x" << block.address << std::endl;
      r << "Block size: 0x" << block.data.size() << std::endl;
      for (uint32_t j = 0; j < 3 && j < image.blocks.size(); j++)
      {
        r << "Block data byte: 0x" << (int)block.data[j] << std::endl;
      }
    }
  }
  return r.str();
}
