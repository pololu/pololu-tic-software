#include <cassert>
#include <thread>
#include <chrono>

#include <programmer.h>
#include <pavrpgm_config.h>

#define USB_VENDOR_ID_POLOLU 0x1FFB
#define USB_PRODUCT_ID_TIC01A 0x00B3

// A setup packet bRequest value from USB 2.0 Table 9-4
#define USB_REQUEST_GET_DESCRIPTOR 6

// A descriptor type from USB 2.0 Table 9-5
#define USB_DESCRIPTOR_TYPE_STRING 3

/** TODO: std::string Tic::convertDeviceResetToString(uint8_t deviceReset)
{
    switch(deviceReset)
    {
    case TIC_RESET_POWER_UP:
        return "Power-on reset";

    case TIC_RESET_BROWNOUT:
        return "Brown-out reset";

    case TIC_RESET_RESET_LINE:
        return "Reset pin driven low";

    case TIC_RESET_WATCHDOG:
        return "Watchdog reset";

    case TIC_RESET_SOFTWARE:
        return "Software reset (bootloader)";

    case TIC_RESET_STACK_OVERFLOW:
        return "Stack overflow";

    case TIC_RESET_STACK_UNDERFLOW:
        return "Stack underflow";

    default:
        return std::string("Unknown code ") + std::to_string(deviceReset) + ".";
    }
}
**/

TicInstance::TicInstance()
{
}

TicInstance::TicInstance(
    libusbp::device usbDevice,
    libusbp::generic_interface usbInterface,
    std::string serialNumber, uint16_t firmwareVersion)
    : usbDevice(usbDevice), usbInterface(usbInterface),
      serialNumber(serialNumber), firmwareVersion(firmwareVersion)
{
}

TicInstance::operator bool() const
{
    return usbInterface;
}

std::string TicInstance::getName()
{
    return "Pololu USB AVR Programmer v2";
}

std::string TicInstance::getOsId() const
{
    return usbDevice.get_os_id();
}

std::string TicInstance::getSerialNumber() const
{
    return serialNumber;
}

uint16_t TicInstance::getFirmwareVersion() const
{
    return firmwareVersion;
}

static uint8_t bcdToDecimal(uint8_t bcd)
{
    return (bcd & 0xF) + 10 * (bcd >> 4);
}

std::string TicInstance::getFirmwareVersionString() const
{
    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%d.%02d",
        getFirmwareVersionMajor(), getFirmwareVersionMinor());
    return std::string(buffer);
}

uint8_t TicInstance::getFirmwareVersionMajor() const
{
    return bcdToDecimal(firmwareVersion >> 8);
}

uint8_t TicInstance::getFirmwareVersionMinor() const
{
    return bcdToDecimal(firmwareVersion & 0xFF);
}

std::vector<TicInstance> ticGetList()
{
    std::vector<TicInstance> list;
    for (const libusbp::device & device : libusbp::list_connected_devices())
    {
        bool match = device.get_vendor_id() == USB_VENDOR_ID_POLOLU &&
            device.get_product_id() == USB_PRODUCT_ID_TIC;
        if (!match) { continue; }

        libusbp::generic_interface usbInterface;
        try
        {
            uint8_t interfaceNumber = 0;
            bool composite = false;
            usbInterface = libusbp::generic_interface(device, interfaceNumber, composite);
        }
        catch(const libusbp::error & error)
        {
            if (error.has_code(LIBUSBP_ERROR_NOT_READY))
            {
                // An error occurred that is normal if the interface is simply
                // not ready to use yet.  Silently ignore it.
                continue;
            }
            throw;
        }
        TicInstance instance(device, usbInterface,
            device.get_serial_number(), device.get_revision());
        list.push_back(instance);
    }
    return list;
}

TicHandle::TicHandle()
{
}

TicHandle::TicHandle(TicInstance instance)
{
    assert(instance);

    if (instance.getFirmwareVersionMajor() > TIC_FIRMWARE_VERSION_MAJOR_MAX)
    {
        throw std::runtime_error(
            "The device has new firmware that is not supported by this software.  "
            "Try using the latest version of this software from " DOCUMENTATION_URL);
    }

    this->instance = instance;
    handle = libusbp::generic_handle(instance.usbInterface);

    // Set a timeout for all control transfers to prevent the CLI from hanging
    // indefinitely if something goes wrong with the USB communication.
    handle.set_timeout(0, 300);
}

void TicHandle::close()
{
    handle.close();
    instance = TicInstance();
}

const TicInstance & TicHandle::getInstance() const
{
    return instance;
}

std::string TicHandle::getFirmwareVersionString()
{
    if (cachedFirmwareVersion.size() > 0)
    {
        return cachedFirmwareVersion;
    }

    std::string version = instance.getFirmwareVersionString();

    // Get the firmware modification string.
    const uint8_t stringIndex = 3;
    size_t transferred = 0;
    uint8_t buffer[64];
    try
    {
        handle.control_transfer(0x80, USB_REQUEST_GET_DESCRIPTOR,
            (USB_DESCRIPTOR_TYPE_STRING << 8) | stringIndex,
            0, buffer, sizeof(buffer), &transferred);
    }
    catch (const libusbp::error & error)
    {
        // Let's make this be a non-fatal error because it's not so important.
        // Just add a question mark so we can tell if something is wrong.
        version += "?";

        // Uncomment this line to debug the error:
        // throw std::runtime_error(std::string("Failed to get firmware modification string."));
    }

    // Add the modification string to the firmware version string, assuming it
    // is ASCII.
    std::string mod;
    for (size_t i = 2; i < transferred; i += 2)
    {
        mod += buffer[i];
    }
    if (mod != "-") { version += mod; }

    cachedFirmwareVersion = version;
    return version;
}

// [all-settings]
ProgrammerSettings TicHandle::getSettings()
{
    ProgrammerSettings settings;
    settings.sckDuration = getRawSetting(PGM04A_SETTING_SCK_DURATION);
    settings.ispFastestPeriod = getRawSetting(PGM04A_SETTING_ISP_FASTEST_PERIOD);
    settings.regulatorMode = getRawSetting(PGM04A_SETTING_REGULATOR_MODE);
    settings.vccOutputEnabled = getRawSetting(PGM04A_SETTING_VCC_OUTPUT_ENABLED) ? 1 : 0;
    settings.vccOutputIndicator = getRawSetting(PGM04A_SETTING_VCC_OUTPUT_INDICATOR) ? 1 : 0;
    settings.lineAFunction = getRawSetting(PGM04A_SETTING_LINE_A_FUNCTION);
    settings.lineBFunction = getRawSetting(PGM04A_SETTING_LINE_B_FUNCTION);
    settings.softwareVersionMajor = getRawSetting(PGM04A_SETTING_SOFTWARE_VERSION_MAJOR);
    settings.softwareVersionMinor = getRawSetting(PGM04A_SETTING_SOFTWARE_VERSION_MINOR);
    settings.hardwareVersion = getRawSetting(PGM04A_SETTING_HARDWARE_VERSION);
    settings.vccVddMaxRange = getRawSetting(PGM04A_SETTING_VCC_VDD_MAX_RANGE)
        * PGM04A_VOLTAGE_UNITS;
    settings.vcc3v3Min = getRawSetting(PGM04A_SETTING_VCC_3V3_MIN) * PGM04A_VOLTAGE_UNITS;
    settings.vcc3v3Max = getRawSetting(PGM04A_SETTING_VCC_3V3_MAX) * PGM04A_VOLTAGE_UNITS;
    settings.vcc5vMin = getRawSetting(PGM04A_SETTING_VCC_5V_MIN) * PGM04A_VOLTAGE_UNITS;
    settings.vcc5vMax = getRawSetting(PGM04A_SETTING_VCC_5V_MAX) * PGM04A_VOLTAGE_UNITS;

    // We don't read the reset polarity here because that gets set by programming
    // software before each session; it is not really a persistent setting
    // and it might be confusing to present it that way.

    return settings;
}

// [all-settings]
void TicHandle::validateSettings(const ProgrammerSettings & settings)
{
    if (settings.sckDuration > 255)
    {
        throw std::runtime_error("The SCK duration should be at most 255.");
    }

    if (settings.ispFastestPeriod < PGM04A_ISP_FASTEST_PERIOD_MIN
        || settings.ispFastestPeriod > PGM04A_ISP_FASTEST_PERIOD_MAX)
    {
        throw std::runtime_error("The ISP fastest period is not valid.");
    }

    if (settings.regulatorMode > PGM04A_REGULATOR_MODE_5V)
    {
        throw std::runtime_error("Invalid regulator mode.");
    }

    if (settings.regulatorMode == PGM04A_REGULATOR_MODE_AUTO &&
        settings.vccOutputEnabled)
    {
        throw std::runtime_error("VCC cannot be an output if the regulator mode is auto.");
    }

    if (settings.lineAFunction > PGM04A_LINE_IS_DTR_RESET)
    {
        throw std::runtime_error("Invalid line A function.");
    }

    if (settings.lineAFunction == PGM04A_LINE_IS_CLOCK)
    {
        throw std::runtime_error("Line A cannot be a clock output.");
    }

    if (settings.lineBFunction > PGM04A_LINE_IS_DTR_RESET)
    {
        throw std::runtime_error("Invalid line B function.");
    }

    if (settings.softwareVersionMajor > 255)
    {
        throw std::runtime_error("Invalid software major version.");
    }

    if (settings.softwareVersionMinor > 255)
    {
        throw std::runtime_error("Invalid software minor version.");
    }

    if (settings.hardwareVersion > 255)
    {
        throw std::runtime_error("Invalid hardware version.");
    }

    if (settings.vccVddMaxRange > maxRepresentableVoltage)
    {
        throw std::runtime_error(
            "The VCC/VDD maximum range cannot be larger than "
            MAX_REPRESENTABLE_VOLTAGE_STR);
    }

    if (settings.vcc3v3Max > maxRepresentableVoltage)
    {
        throw std::runtime_error(
            "The VCC 3.3 V maximum cannot be larger than "
            MAX_REPRESENTABLE_VOLTAGE_STR);
    }

    if (settings.vcc5vMax > maxRepresentableVoltage)
    {
        throw std::runtime_error(
            "The VCC 5 V maximum cannot be larger than "
            MAX_REPRESENTABLE_VOLTAGE_STR);
    }

    if (settings.vcc3v3Min > settings.vcc3v3Max)
    {
        throw std::runtime_error(
            "The VCC 3.3 V minimum cannot be greater than the maximum.");
    }

    if (settings.vcc5vMin > settings.vcc5vMax)
    {
        throw std::runtime_error(
            "The VCC 5 V minimum cannot be greater than the maximum.");
    }
}

// Divide by PGM04A_VOLTAGE_UNITs, rounding to the nearest integer.
static uint8_t convertMvToRawUnits(uint32_t mv)
{
    uint32_t x = (mv + PGM04A_VOLTAGE_UNITS / 2 - 1) / PGM04A_VOLTAGE_UNITS;
    if (x > 0xFF)
    {
        x = 0xFF;

        // Bounds are checked before we get to this point,
        // so this block should never run.
        assert(0);
    }
    return x;
}

// [all-settings]
void TicHandle::applySettings(const ProgrammerSettings & settings)
{
    validateSettings(settings);

    // We set vccOutputEnabled in a special way to ensure that if it is changing,
    // we won't accidentally output the wrong voltage on VCC for some time.
    if (!settings.vccOutputEnabled)
    {
        setRawSetting(PGM04A_SETTING_VCC_OUTPUT_ENABLED, 0);
    }

    setRawSetting(PGM04A_SETTING_SCK_DURATION, settings.sckDuration);
    setRawSetting(PGM04A_SETTING_ISP_FASTEST_PERIOD, settings.ispFastestPeriod);
    setRawSetting(PGM04A_SETTING_REGULATOR_MODE, settings.regulatorMode);
    setRawSetting(PGM04A_SETTING_VCC_OUTPUT_INDICATOR, settings.vccOutputIndicator);
    setRawSetting(PGM04A_SETTING_LINE_A_FUNCTION, settings.lineAFunction);
    setRawSetting(PGM04A_SETTING_LINE_B_FUNCTION, settings.lineBFunction);
    setRawSetting(PGM04A_SETTING_SOFTWARE_VERSION_MAJOR, settings.softwareVersionMajor);
    setRawSetting(PGM04A_SETTING_SOFTWARE_VERSION_MINOR, settings.softwareVersionMinor);
    setRawSetting(PGM04A_SETTING_HARDWARE_VERSION, settings.hardwareVersion);
    setRawSetting(PGM04A_SETTING_VCC_VDD_MAX_RANGE,
        convertMvToRawUnits(settings.vccVddMaxRange));
    setRawSetting(PGM04A_SETTING_VCC_3V3_MIN,
        convertMvToRawUnits(settings.vcc3v3Min));
    setRawSetting(PGM04A_SETTING_VCC_3V3_MAX,
        convertMvToRawUnits(settings.vcc3v3Max));
    setRawSetting(PGM04A_SETTING_VCC_5V_MIN,
        convertMvToRawUnits(settings.vcc5vMin));
    setRawSetting(PGM04A_SETTING_VCC_5V_MAX,
        convertMvToRawUnits(settings.vcc5vMax));

    if (settings.vccOutputEnabled)
    {
        setRawSetting(PGM04A_SETTING_VCC_OUTPUT_ENABLED, 1);
    }
}

void TicHandle::restoreDefaults()
{
    setRawSetting(PGM04A_SETTING_NOT_INITIALIZED, 0xFF);

    // The request above returns before the settings are actually initialized.
    // Wait until the programmer succeeds in reinitializing its settings.
    uint32_t timeMs = 0;
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        timeMs += 10;

        uint8_t notInitialized = getRawSetting(PGM04A_SETTING_NOT_INITIALIZED);
        if (!notInitialized)
        {
            break;
        }

        if (timeMs > 300)
        {
            throw std::runtime_error(
                "A timeout occurred while resetting to default settings.");
        }
    }
}

ProgrammerVariables TicHandle::getVariables()
{
    ProgrammerVariables vars;

    vars.lastDeviceReset = getRawVariable(PGM04A_VARIABLE_LAST_DEVICE_RESET);

    vars.programmingError = getRawVariable(PGM04A_VARIABLE_PROGRAMMING_ERROR);

    vars.targetVccMeasuredMinMv =
        getRawVariable(PGM04A_VARIABLE_TARGET_VCC_MEASURED_MIN) * PGM04A_VOLTAGE_UNITS;

    vars.targetVccMeasuredMaxMv =
        getRawVariable(PGM04A_VARIABLE_TARGET_VCC_MEASURED_MAX) * PGM04A_VOLTAGE_UNITS;

    vars.programmerVddMeasuredMinMv =
        getRawVariable(PGM04A_VARIABLE_PROGRAMMER_VDD_MEASURED_MIN) * PGM04A_VOLTAGE_UNITS;

    vars.programmerVddMeasuredMaxMv =
        getRawVariable(PGM04A_VARIABLE_PROGRAMMER_VDD_MEASURED_MAX) * PGM04A_VOLTAGE_UNITS;

    vars.hasResultsFromLastProgramming =
        (vars.programmingError != 0) ||
        (vars.targetVccMeasuredMinMv != 255 * PGM04A_VOLTAGE_UNITS) ||
        (vars.targetVccMeasuredMaxMv != 0) ||
        (vars.programmerVddMeasuredMinMv != 255 * PGM04A_VOLTAGE_UNITS) ||
        (vars.programmerVddMeasuredMaxMv != 0);

    vars.targetVccMv =
        getRawVariable(PGM04A_VARIABLE_TARGET_VCC) * PGM04A_VOLTAGE_UNITS;

    vars.programmerVddMv =
        getRawVariable(PGM04A_VARIABLE_PROGRAMMER_VDD) * PGM04A_VOLTAGE_UNITS;

    vars.regulatorLevel = getRawVariable(PGM04A_VARIABLE_REGULATOR_LEVEL);

    vars.inProgrammingMode = getRawVariable(PGM04A_VARIABLE_IN_PROGRAMMING_MODE) ? 1 : 0;

    return vars;
}

ProgrammerDigitalReadings TicHandle::digitalRead()
{
    uint8_t buffer[3];
    size_t transferred;
    try
    {
        handle.control_transfer(0xC0, PGM04A_REQUEST_DIGITAL_READ,
            0, 0, &buffer, 3, &transferred);
    }
    catch (const libusbp::error & error)
    {
        throw std::runtime_error(std::string("Failed to get a variable.  ")
            + error.message());
    }

    if (transferred != 3)
    {
        throw std::runtime_error(std::string("Failed to get a variable.  ") +
            "Expected 3 bytes, got " + std::to_string(transferred));
    }

    ProgrammerDigitalReadings readings;
    readings.portA = buffer[0];
    readings.portB = buffer[1];
    readings.portC = buffer[2];
    return readings;
}

bool pgm03aPresent()
{
    for (const libusbp::device & device : libusbp::list_connected_devices())
    {
        bool isPgm03a = device.get_vendor_id() == USB_VENDOR_ID_POLOLU &&
            device.get_product_id() == USB_PRODUCT_ID_PGM03A;
        if (isPgm03a) { return true; }
    }
    return false;
}


/** This message doesn't look great when printed in a CLI because it has lines
 * longer than 80 characters.  But if we add wrapping to it then it will look
 * bad when it is printed in a messagebox in a typical GUI. **/
const char * pgm03aMessage =
    "This utility only supports the Pololu USB AVR Programmer v2 "
    "(blue-colored, labeled \"pgm04a\").\n"
    "\n"
    "It looks you have an older programmer, the Pololu USB AVR Programmer (pgm03a).  "
    "You can find documentation and software for the older programmer here:\n"
    "\n"
    "https://www.pololu.com/docs/0J36";
