#include "bootloader.h"
#include "tinyxml2.h"
#include <sstream>

#define USB_VENDOR_ID_POLOLU 0x1FFB

static int hexDigitValue(unsigned char c)
{
    if (c >= 'a' && c <= 'f') { return c - 'a' + 10; }
    if (c >= 'A' && c <= 'F') { return c - 'A' + 10; }
    if (c >= '0' && c <= '9') { return c - '0'; }
    return -1;
}

static uint8_t readHexByte(std::istream & s)
{
    std::istream::sentry sentry(s, true);
    if (sentry)
    {
        char c1 = 0, c2 = 0;
        s.get(c1);
        s.get(c2);

        if (s.fail())
        {
            if (s.eof())
            {
                throw std::runtime_error("Unexpected end of line.");
            }
            else
            {
                throw std::runtime_error("Failed to read hex digit.");
            }
        }

        int v1 = hexDigitValue(c1);
        int v2 = hexDigitValue(c2);

        if (v1 < 0 || v2 < 0)
        {
            throw std::runtime_error("Invalid hex digit.");
        }

        return v1 * 16 + v2;
    }
    return 0;
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

/** Unfortunately, TinyXML2 does not provide good ways of reporting XML parsing
 * errors.  We could try to use GetErrorStr1() and GetErrorStr2(), but those
 * just return pointers to parts of the document, and it is not OK to print
 * parts of the document without carefully escaping special characters in it,
 * which is probably not worthwhile. */
static void throwIfError(const tinyxml2::XMLDocument & doc)
{
    if(!doc.Error()) { return; }

    std::string msg("XML error.");

    // doc.ErrorName is not available in Ubuntu's libtinyxml2-dev/trusty-backports package
    // (2.1.0).  But it would be nice to use it eventually once everyone has upgraded:
    // std::string msg("XML error: ");
    // msg += doc.ErrorName();
    // msg += ".";
    throw std::runtime_error(msg);
}

// Note: Unfortunately, this function will not notice if there is extra junk at
// the end of the string.  It might be good to change that eventually.
static bool niceStrToULong(const char * cstr, unsigned long * out)
{
    assert(cstr != NULL);
    assert(out != NULL);

    *out = 0;

    std::string string = cstr;

    try
    {
        *out = stoul(string, 0, 16);
        return false;
    }
    catch(const std::invalid_argument & e) { }
    catch(const std::out_of_range & e) { }
    return true;
}

static FirmwareArchive::Block processXmlBlock(
    const tinyxml2::XMLElement * element)
{
    assert(element != NULL);

    FirmwareArchive::Block block;

    // Get the address.
    const char * addressCStr = element->Attribute("address");
    if (addressCStr == NULL)
    {
        throw std::runtime_error("A block is missing an address attribute.");
    }
    unsigned long address;
    if (niceStrToULong(addressCStr, &address))
    {
        throw std::runtime_error("A block has an invalid address attribute.");
    }
    block.address = address;

    // Get the contents.
    const char * contentsCStr = element->GetText();
    if (contentsCStr == NULL)
    {
        throw std::runtime_error("A block has missing or invalid contents.");
    }

    std::string contents(contentsCStr);

    if ((contents.size() % 2) != 0)
    {
        throw std::runtime_error("A block has an odd number of characters.");
    }

    uint32_t byteCount = contents.size() / 2;

    std::istringstream stream(contents);
    block.data.resize(byteCount);
    for (uint32_t i = 0; i < byteCount; i++)
    {
        block.data[i] = readHexByte(stream);
    }

    return block;
}

static FirmwareArchive::Image processXmlFirmwareImage(
    const tinyxml2::XMLElement * element)
{
    assert(element != NULL);

    FirmwareArchive::Image image;

    // Get the product ID.
    const char * productCStr = element->Attribute("product");
    if (productCStr == NULL)
    {
        throw std::runtime_error("An image is missing a product attribute.");
    }
    unsigned long productId;
    if (niceStrToULong(productCStr, &productId) || productId > 0xffff)
    {
        throw std::runtime_error("Invalid product attribute for firmware image.");
    }
    image.usbProductId = productId;

    // FMI files don't store the vendor ID so we assume they are for Pololu
    // products.  But we insulate higher-level code from this detail.
    image.usbVendorId = USB_VENDOR_ID_POLOLU;

    // Get the upload type.
    image.uploadType = UPLOAD_TYPE_STANDARD;
    const char * uploadTypeCStr = element->Attribute("uploadType");
    if (uploadTypeCStr)
    {
        std::string uploadType = uploadTypeCStr;
        if (uploadType == "Standard")
        {
            image.uploadType = UPLOAD_TYPE_STANDARD;
        }
        else if (uploadType == "DeviceSpecific")
        {
            image.uploadType = UPLOAD_TYPE_DEVICE_SPECIFIC;
        }
        else if (uploadType == "Plain")
        {
            image.uploadType = UPLOAD_TYPE_PLAIN;
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

        image.blocks.push_back(processXmlBlock(element));
    }

    if (image.blocks.empty())
    {
        throw std::runtime_error("An image has no blocks in it.");
    }

    return image;
}

void FirmwareArchive::Data::processXml(std::string string)
{
    // Parse the string as XML.
    tinyxml2::XMLDocument doc;
    doc.Parse(string.c_str(), string.size());
    throwIfError(doc);

    // Check the FirmwareArchive element.
    tinyxml2::XMLElement * root = doc.RootElement();
    if (std::string(root->Name()) != "FirmwareArchive")
    {
        throw std::runtime_error("The root element has an invalid name.");
    }

    // Check the FirmwareArchive format attribute.  If we add any information to
    // the FMI format that *must* be processed by all readers, we will increment
    // the major version number.  If we add any information that is optional, we
    // might increment the minor version.  Therefore, we don't need to check the
    // minor version here, and we don't need to worry about unrecognized
    // attributes or tags, but we must check the major version.
    std::string format = root->Attribute("format");
    std::vector<std::string> parts = split(format, '.');
    if (parts[0] != "1")
    {
        throw std::runtime_error(
            "The firmware archive format is different than expected.  "
            "Try installing the latest version of this software.");
    }

    // Get the FirmwareArchive name attribute.
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

        images.push_back(processXmlFirmwareImage(element));
    }

    if (images.empty())
    {
        throw std::runtime_error("The firmware archive has no images.");
    }
}

void FirmwareArchive::Data::readFromFile(std::istream & file,
    const char * fileName)
{
    // Read the entire file into a string.
    std::stringstream buffer;
    buffer << file.rdbuf();
    if (file.fail())
    {
        throw std::runtime_error(std::string(fileName) + ": error reading.");
    }

    try
    {
        processXml(buffer.str());
    }
    catch(const std::runtime_error & e)
    {
        throw std::runtime_error(std::string(fileName) +
            ": " + e.what());
    }
}
