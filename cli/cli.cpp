#include <tic.hpp>
#include "config.h"
#include "arg_reader.h"
#include "exit_codes.h"
#include "exception_with_exit_code.h"

#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <bitset>
#include <cassert>

// [all-settings]
static const char help[] =
    CLI_NAME ": Pololu USB AVR Programmer v2 Command-line Utility\n"
    "Version " SOFTWARE_VERSION_STRING "\n"
    "Usage: " CLI_NAME " OPTIONS\n"
    "\n"
    "General options:\n"
    "  -s, --status                Show programmer settings and info.\n"
    "  -d SERIALNUMBER             Specifies the serial number of the programmer.\n"
    "  --list                      List programmers connected to computer.\n"
    "  --prog-port                 Print the name of the programming serial port.\n"
    "  --ttl-port                  Print the name of the TTL serial port.\n"
    "  -h, --help                  Show this help screen.\n"
    "\n"
    "Options for changing settings:\n"
    "  --regulator-mode MODE       Sets programmer's operating voltage.\n"
    "                              auto - choose 3.3 V or 5 V based on target VCC\n"
    "                              3v3  - always 3.3 V\n"
    "                              5v   - switch to 5 V soon after power up\n"
    "  --vcc-output OPTION         Choose whether the programmer supplies power\n"
    "                              to target VCC.  Options: enabled, disabled.\n"
    "  --vcc-output-ind OPTION     Sets how to indicate that VCC is an output.\n"
    "                              blinking - yellow LED(s) blink at 8 Hz (default)\n"
    "                              steady - yellow LED(s) are on constantly\n"
    "  --freq NUM                  Sets the ISP frequency (in units of kHz).\n"
    "                              Must be typed exactly as shown below.\n"
    "                              Suggested values: 3000, 2400, 2000, 1714\n"
    "                              1500, 1200, 1000, 750, 444, 114 (default),\n"
    "                              28.3, 14.0, 6.98, 3.47, 1.744, 1.465.\n"
    "  --max-freq NUM              Sets the max ISP frequency (in units of kHz).\n"
    "                              Must be typed exactly as shown below.\n"
    "                              Suggested values: 3000, 2400, 2000,\n"
    "                              1714 (default), 1500, 1200, 1000, 750.\n"
    "  --line-a FUNC               Set the function of line A.  Valid FUNCs are:\n"
    "                              none, cd, dsr, dtr, rts, dtr-reset.\n"
    "  --line-b FUNC               Set the function of line B.  Valid FUNCs are:\n"
    "                              none, cd, dsr, dtr, rts, clock, dtr-reset.\n"
    "  --vcc-vdd-max-range MV      Sets the maximum allowed range for VCC and VDD\n"
    "                              while programming.\n"
    "  --vcc-3v3-min MV            Minimum allowed VCC while programming at 3.3 V.\n"
    "  --vcc-3v3-max MV            Maximum allowed VCC while programming at 3.3 V.\n"
    "  --vcc-5v-min MV             Minimum allowed VCC while programming at 5 V.\n"
    "  --vcc-5v-max MV             Maximum allowed VCC while programming at 5 V.\n"
    "  --sw-minor HEXNUM           Set STK500 software version minor (in hex)\n"
    "  --sw-major HEXNUM           Set STK500 software version major (in hex)\n"
    "  --hw HEXNUM                 Set STK500 software hardware version (in hex)\n"
    "  --restore-defaults          Restore factory settings\n"
    "\n"
    "This utility only supports the Pololu USB AVR Programmer v2\n"
    "(blue-colored, labeled \"pgm04a\").\n"
    "\n"
    "MV should be a voltage in millivolts.  For example, \"3400\" means 3.4 V.\n"
    "\n"
    "For more help, see: " DOCUMENTATION_URL "\n"
    "\n";

// Note: The arguments that are entered as a number by the user are all
// uint32_t.  If we made them be their proper types, that would mean adding more
// information to the CLI code that code instead be in the library, and it means
// a typical user would see a wider variety of error messages.
// [all-settings]
struct Arguments
{
    bool showStatus = false;

    bool serialNumberSpecified = false;
    std::string serialNumber;

    bool showList = false;

    bool regulatorModeSpecified = false;
    uint8_t regulatorMode;

    bool vccOutputEnabledSpecified = false;
    bool vccOutputEnabled;

    bool vccOutputIndicatorSpecified = false;
    uint8_t vccOutputIndicator;

    bool frequencySpecified = false;
    std::string frequencyName;

    bool maxFrequencySpecified = false;
    std::string maxFrequencyName;

    bool lineAFunctionSpecified = false;
    uint8_t lineAFunction;

    bool lineBFunctionSpecified = false;
    uint8_t lineBFunction;

    bool softwareVersionMinorSpecified = false;
    uint32_t softwareVersionMinor;

    bool softwareVersionMajorSpecified = false;
    uint32_t softwareVersionMajor;

    bool hardwareVersionSpecified = false;
    uint32_t hardwareVersion;

    bool vccVddMaxRangeSpecified = false;
    uint32_t vccVddMaxRange;

    bool vcc3v3MinSpecified = false;
    uint32_t vcc3v3Min;

    bool vcc3v3MaxSpecified = false;
    uint32_t vcc3v3Max;

    bool vcc5vMinSpecified = false;
    uint32_t vcc5vMin;

    bool vcc5vMaxSpecified = false;
    uint32_t vcc5vMax;

    bool restoreDefaults = false;

    bool printProgrammingPort = false;

    bool printTtlPort = false;

    bool showHelp = false;

    bool digitalRead = false;

    // [all-settings]
    bool settingsSpecified() const
    {
        return regulatorModeSpecified ||
            vccOutputEnabledSpecified ||
            vccOutputIndicatorSpecified ||
            frequencySpecified ||
            maxFrequencySpecified ||
            lineAFunctionSpecified ||
            lineBFunctionSpecified ||
            softwareVersionMinorSpecified ||
            softwareVersionMajorSpecified ||
            hardwareVersionSpecified ||
            vccVddMaxRangeSpecified ||
            vcc3v3MinSpecified ||
            vcc3v3MaxSpecified ||
            vcc5vMinSpecified ||
            vcc5vMaxSpecified ||
            restoreDefaults;
    }

    bool actionSpecified() const
    {
        return showStatus ||
            showList ||
            settingsSpecified() ||
            printProgrammingPort ||
            printTtlPort ||
            showHelp ||
            digitalRead;
    }
};

/* Contains the logic for finding programmers. */
class ProgrammerSelector
{
public:
    void specifySerialNumber(const std::string & serialNumber)
    {
        assert(!listInitialized);
        this->serialNumber = serialNumber;
        this->serialNumberSpecified = true;
    }

    std::vector<ProgrammerInstance> listProgrammers()
    {
        if (listInitialized) { return list; }

        list.clear();
        for (const ProgrammerInstance & instance : programmerGetList())
        {
            if (serialNumberSpecified &&
                instance.getSerialNumber() != serialNumber)
            {
                continue;
            }

            list.push_back(instance);
        }
        listInitialized = true;
        return list;
    }

    ProgrammerInstance selectProgrammer()
    {
        if (programmer) { return programmer; }

        auto list = listProgrammers();
        if (list.size() == 0)
        {
            throw deviceNotFoundError();
        }

        if (list.size() > 1)
        {
            throw deviceMultipleFoundError();
        }

        programmer = list[0];
        return programmer;
    }

private:

    std::string deviceNotFoundMessage() const
    {
        std::string r = "No programmer was found";

        if (serialNumberSpecified)
        {
            r += std::string(" with serial number '") +
                serialNumber + "'";
        }

        r += ".";
        return r;
    }

    ExceptionWithExitCode deviceNotFoundError() const
    {
        return ExceptionWithExitCode(PAVRPGM_ERROR_DEVICE_NOT_FOUND,
            deviceNotFoundMessage());
    }

    ExceptionWithExitCode deviceMultipleFoundError() const
    {
        return ExceptionWithExitCode(PAVRPGM_ERROR_DEVICE_MULTIPLE_FOUND,
            "There are multiple qualifying programmers connected to this computer.\n"
            "Use the -d option to specify which device you want to use,\n"
            "or disconnect the others.");
    }

    bool serialNumberSpecified = false;
    std::string serialNumber;

    bool listInitialized = false;
    std::vector<ProgrammerInstance> list;

    ProgrammerInstance programmer;
};

// Converts a string to an unsigned long, returning true if there is an error.
static bool niceStrToULong(std::string string, unsigned long & out, int base = 10)
{
    out = 0;
    size_t pos;

    try
    {
        out = stoul(string, &pos, base);
        return (pos != string.size());
    }
    catch(const std::invalid_argument & e) { }
    catch(const std::out_of_range & e) { }
    return true;
}

void parseArgUInt32(ArgReader & argReader, uint32_t & out, int base = 10)
{
    const char * valueCStr = argReader.next();
    if (valueCStr == NULL)
    {
        throw ExceptionWithExitCode(PAVRPGM_ERROR_BAD_ARGS,
            "Expected a number after '" + std::string(argReader.last()) + "'.");
    }

    unsigned long value;
    if (niceStrToULong(valueCStr, value, base))
    {
        throw ExceptionWithExitCode(PAVRPGM_ERROR_BAD_ARGS,
            "The number after '" + std::string(argReader.last()) + "' is invalid.");
    }

    if (value > std::numeric_limits<uint32_t>::max())
    {
        throw ExceptionWithExitCode(PAVRPGM_ERROR_BAD_ARGS,
            "The number after '" + std::string(argReader.last()) + "' is too large.");
    }

    out = value;
}

void parseArgLineFunction(ArgReader & argReader, uint8_t & out)
{
    const char * valueCStr = argReader.next();
    if (valueCStr == NULL)
    {
        throw ExceptionWithExitCode(PAVRPGM_ERROR_BAD_ARGS,
            "Expected a line function name after '" +
            std::string(argReader.last()) + "'.");
    }

    std::string value = valueCStr;

    if (value == "none" || value == "NONE")
    {
        out = PGM04A_LINE_IS_NOTHING;
    }
    else if (value == "cd" || value == "CD")
    {
        out = PGM04A_LINE_IS_CD;
    }
    else if (value == "dsr" || value == "DSR")
    {
        out = PGM04A_LINE_IS_DSR;
    }
    else if (value == "dtr" || value == "DTR")
    {
        out = PGM04A_LINE_IS_DTR;
    }
    else if (value == "rts" || value == "RTS")
    {
        out = PGM04A_LINE_IS_RTS;
    }
    else if (value == "clock" || value == "CLOCK")
    {
        out = PGM04A_LINE_IS_CLOCK;
    }
    else if (value == "dtr-reset" || value == "DTR-RESET" || value == "DTR-reset")
    {
        out = PGM04A_LINE_IS_DTR_RESET;
    }
    else
    {
        throw ExceptionWithExitCode(PAVRPGM_ERROR_BAD_ARGS,
            "The line function name after '" +
            std::string(argReader.last()) + "' is invalid.");
    }
}

static void parseArgSerialNumber(ArgReader & argReader, Arguments & args)
{
    const char * valueCStr = argReader.next();
    if (valueCStr == NULL)
    {
        throw ExceptionWithExitCode(PAVRPGM_ERROR_BAD_ARGS,
            "Expected a serial number after '"
            + std::string(argReader.last()) + "'.");
    }
    if (valueCStr[0] == 0)
    {
        throw ExceptionWithExitCode(PAVRPGM_ERROR_BAD_ARGS,
            "An empty serial number was specified.");
    }

    args.serialNumberSpecified = true;
    args.serialNumber = valueCStr;
}

static void parseArgString(ArgReader & argReader, std::string & str)
{
    const char * valueCStr = argReader.next();
    if (valueCStr == NULL)
    {
        throw ExceptionWithExitCode(PAVRPGM_ERROR_BAD_ARGS,
            "Expected an argument after '" +
            std::string(argReader.last()) + "'.");
    }
    str = valueCStr;
}

static void parseArgRegulatorMode(ArgReader & argReader, Arguments & args)
{
    const char * valueCStr = argReader.next();
    std::string value = valueCStr == NULL ? "" : valueCStr;

    if (value == "auto")
    {
        args.regulatorMode = PGM04A_REGULATOR_MODE_AUTO;
        args.regulatorModeSpecified = true;
    }
    else if (value == "5v" || value == "5V")
    {
        args.regulatorMode = PGM04A_REGULATOR_MODE_5V;
        args.regulatorModeSpecified = true;
    }
    else if (value == "3v3" || value == "3V3")
    {
        args.regulatorMode = PGM04A_REGULATOR_MODE_3V3;
        args.regulatorModeSpecified = true;
    }
    else
    {
        throw ExceptionWithExitCode(PAVRPGM_ERROR_BAD_ARGS,
            "Expected 'auto', '5v', or '3v3' after '"
            + std::string(argReader.last()) + "'.");
    }
}

static void parseArgVccOutput(ArgReader & argReader, Arguments & args)
{
    const char * valueCStr = argReader.next();
    std::string value = valueCStr == NULL ? "" : valueCStr;

    if (value == "disabled" || value == "Disabled")
    {
        args.vccOutputEnabled = false;
        args.vccOutputEnabledSpecified = true;
    }
    else if (value == "enabled" || value == "Enabled")
    {
        args.vccOutputEnabled = true;
        args.vccOutputEnabledSpecified = true;
    }
    else
    {
        throw ExceptionWithExitCode(PAVRPGM_ERROR_BAD_ARGS,
            "Expected 'disabled' or 'enabled' after '"
            + std::string(argReader.last()) + "'.");
    }
}

static void parseArgVccOutputIndicator(ArgReader & argReader, Arguments & args)
{
    const char * valueCStr = argReader.next();
    std::string value = valueCStr == NULL ? "" : valueCStr;

    if (value == "blinking" || value == "Blinking")
    {
        args.vccOutputIndicator = PGM04A_VCC_OUTPUT_INDICATOR_BLINKING;
        args.vccOutputIndicatorSpecified = true;
    }
    else if (value == "steady" || value == "Steady")
    {
        args.vccOutputIndicator = PGM04A_VCC_OUTPUT_INDICATOR_STEADY;
        args.vccOutputIndicatorSpecified = true;
    }
    else
    {
        throw ExceptionWithExitCode(PAVRPGM_ERROR_BAD_ARGS,
            "Expected 'blinking' or 'steady' after '"
            + std::string(argReader.last()) + "'.");
    }
}

// [all-settings]
static Arguments parseArgs(int argc, char ** argv)
{
    ArgReader argReader(argc, argv);
    Arguments args;

    while (1)
    {
        const char * argCStr = argReader.next();
        if (argCStr == NULL)
        {
            break;  // Done reading arguments.
        }

        std::string arg = argCStr;

        if (arg == "-d")
        {
            parseArgSerialNumber(argReader, args);
        }
        else if (arg == "-s" || arg == "--status")
        {
            args.showStatus = true;
        }
        else if (arg == "--list")
        {
            args.showList = true;
        }
        else if (arg == "--regulator-mode")
        {
            parseArgRegulatorMode(argReader, args);
        }
        else if (arg == "--vcc-output")
        {
            parseArgVccOutput(argReader, args);
        }
        else if (arg == "--vcc-output-ind" || arg == "--vcc-output-indicator")
        {
            parseArgVccOutputIndicator(argReader, args);
        }
        else if (arg == "--freq")
        {
            parseArgString(argReader, args.frequencyName);
            args.frequencySpecified = true;
        }
        else if (arg == "--max-freq")
        {
            parseArgString(argReader, args.maxFrequencyName);
            args.maxFrequencySpecified = true;
        }
        else if (arg == "--line-a")
        {
            parseArgLineFunction(argReader, args.lineAFunction);
            args.lineAFunctionSpecified = true;
        }
        else if (arg == "--line-b")
        {
            parseArgLineFunction(argReader, args.lineBFunction);
            args.lineBFunctionSpecified = true;
        }
        else if (arg == "--sw-minor")
        {
            parseArgUInt32(argReader, args.softwareVersionMinor, 16);
            args.softwareVersionMinorSpecified = true;
        }
        else if (arg == "--sw-major")
        {
            parseArgUInt32(argReader, args.softwareVersionMajor, 16);
            args.softwareVersionMajorSpecified = true;
        }
        else if (arg == "--hw")
        {
            parseArgUInt32(argReader, args.hardwareVersion, 16);
            args.hardwareVersionSpecified = true;
        }
        else if (arg == "--vcc-vdd-max-range")
        {
            parseArgUInt32(argReader, args.vccVddMaxRange);
            args.vccVddMaxRangeSpecified = true;
        }
        else if (arg == "--vcc-3v3-min")
        {
            parseArgUInt32(argReader, args.vcc3v3Min);
            args.vcc3v3MinSpecified = true;
        }
        else if (arg == "--vcc-3v3-max")
        {
            parseArgUInt32(argReader, args.vcc3v3Max);
            args.vcc3v3MaxSpecified = true;
        }
        else if (arg == "--vcc-5v-min")
        {
            parseArgUInt32(argReader, args.vcc5vMin);
            args.vcc5vMinSpecified = true;
        }
        else if (arg == "--vcc-5v-max")
        {
            parseArgUInt32(argReader, args.vcc5vMax);
            args.vcc5vMaxSpecified = true;
        }
        else if (arg == "--restore-defaults")
        {
            args.restoreDefaults = true;
        }
        else if (arg == "--prog-port")
        {
            args.printProgrammingPort = true;
        }
        else if (arg == "--ttl-port")
        {
            args.printTtlPort = true;
        }
        else if (arg == "-h" || arg == "--help" ||
            arg == "--h" || arg == "-help" || arg == "/help" || arg == "/h")
        {
            args.showHelp = true;
        }
        else if (arg == "-r" || arg == "--digital-read")
        {
            args.digitalRead = true;
        }
        else
        {
            throw ExceptionWithExitCode(PAVRPGM_ERROR_BAD_ARGS,
                std::string("Unknown option: '") + arg + "'.");
        }
    }
    return args;
}

static void adjustArguments(Arguments & args)
{
    if (!args.actionSpecified())
    {
        // The user did not explicitly specify an action on the command-line.
        // Show the help screen.
        args.showHelp = true;
    }
}

static void printProgrammerList(ProgrammerSelector & selector)
{
    for (const ProgrammerInstance & instance : selector.listProgrammers())
    {
        std::cout << std::left << std::setfill(' ');
        std::cout << std::setw(17) << instance.getSerialNumber() + "," << " ";
        std::cout << std::setw(45) << instance.getName();
        std::cout << std::endl;
    }
}

// [all-settings]
static void printProgrammerStatus(ProgrammerSelector & selector)
{
    ProgrammerInstance instance = selector.selectProgrammer();
    ProgrammerHandle handle(instance);
    std::string firmwareVersion = handle.getFirmwareVersionString();
    ProgrammerSettings settings = handle.getSettings();
    ProgrammerVariables variables = handle.getVariables();

    // The output here is compatible with YAML so that people can more easily
    // write scripts the use it.

    int leftColumnWidth = 41;
    auto leftColumn = std::setw(leftColumnWidth);

    std::string frequency = Programmer::getFrequencyName(
        settings.sckDuration, settings.ispFastestPeriod);
    std::string maxFrequency = Programmer::getMaxFrequencyName(
        settings.ispFastestPeriod);

    std::cout << std::left << std::setfill(' ');

    std::cout << leftColumn << "Name: "
              << handle.getInstance().getName() << std::endl;

    std::cout << leftColumn << "Serial number: "
              << handle.getInstance().getSerialNumber() << std::endl;

    std::cout << leftColumn << "Firmware version: "
              << firmwareVersion << std::endl;

    std::cout << leftColumn << "Programming port: "
              << handle.getInstance().tryGetProgrammingPortName() << std::endl;

    std::cout << leftColumn << "TTL port: "
              << handle.getInstance().tryGetTtlPortName() << std::endl;

    std::cout << std::endl;

    std::cout << "Settings:" << std::endl;

    std::cout << leftColumn << "  ISP frequency (kHz): "
              << frequency << std::endl;

    std::cout << leftColumn << "  Max ISP frequency (kHz): "
              << maxFrequency << std::endl;

    std::cout << leftColumn << "  Regulator mode: "
              << Programmer::convertRegulatorModeToString(settings.regulatorMode)
              << std::endl;

    std::cout << leftColumn << "  VCC output: "
              << (settings.vccOutputEnabled ? "Enabled" : "Disabled") << std::endl;

    std::cout << leftColumn << "  VCC output indicator: "
              << (settings.vccOutputIndicator ? "Steady" : "Blinking") << std::endl;

    std::cout << leftColumn << "  Line A function: "
              << Programmer::convertLineFunctionToString(settings.lineAFunction)
              << std::endl;

    std::cout << leftColumn << "  Line B function: "
              << Programmer::convertLineFunctionToString(settings.lineBFunction)
              << std::endl;

    std::cout << leftColumn << "  VCC/VDD maximum range (mV): "
              << settings.vccVddMaxRange << std::endl;

    std::cout << leftColumn << "  VCC 3.3 V minimum (mV): "
              << settings.vcc3v3Min << std::endl;

    std::cout << leftColumn << "  VCC 3.3 V maximum (mV): "
              << settings.vcc3v3Max << std::endl;

    std::cout << leftColumn << "  VCC 5 V minimum (mV): "
              << settings.vcc5vMin << std::endl;

    std::cout << leftColumn << "  VCC 5 V maximum (mV): "
              << settings.vcc5vMax << std::endl;

    std::cout << std::hex << std::uppercase;

    std::cout << leftColumn << "  STK500 hardware version: "
              << settings.hardwareVersion << std::endl;

    std::cout << leftColumn << "  STK500 software version: "
              << settings.softwareVersionMajor << "."
              << settings.softwareVersionMinor << std::endl;

    std::cout << std::dec;

    std::cout << std::endl;

    if (variables.hasResultsFromLastProgramming)
    {
        std::cout << "Results from last programming:" << std::endl;
        std::cout << leftColumn << "  Programming error: "
                  << Programmer::convertProgrammingErrorToShortString(variables.programmingError)
                  << std::endl;
        std::cout << leftColumn << "  Target VCC measured minimum (mV): "
                  << variables.targetVccMeasuredMinMv
                  << std::endl;
        std::cout << leftColumn << "  Target VCC measured maximum (mV): "
                  << variables.targetVccMeasuredMaxMv
                  << std::endl;
        std::cout << leftColumn << "  Programmer VDD measured minimum (mV): "
                  << variables.programmerVddMeasuredMinMv
                  << std::endl;
        std::cout << leftColumn << "  Programmer VDD measured maximum (mV): "
                  << variables.programmerVddMeasuredMaxMv
                  << std::endl;
    }
    else
    {
        std::cout << leftColumn << "Results from last programming: "
                  << "N/A" << std::endl;
    }

    std::cout << std::endl;

    std::cout << "Current status:" << std::endl;
    std::cout << leftColumn << "  Target VCC (mV): "
              << variables.targetVccMv
              << std::endl;
    std::cout << leftColumn << "  Programmer VDD (mV): "
              << variables.programmerVddMv
              << std::endl;
    std::cout << leftColumn << "  VDD regulator set point: "
              << Programmer::convertRegulatorLevelToString(variables.regulatorLevel)
              << std::endl;

    std::cout << leftColumn << "  Last device reset: "
              << Programmer::convertDeviceResetToString(variables.lastDeviceReset)
              << std::endl;
}

// Print the name of the programming serial port (e.g. "COM 4").
void printProgrammingPort(ProgrammerSelector & selector)
{
    ProgrammerInstance instance = selector.selectProgrammer();
    std::string programmingPortName = instance.getProgrammingPortName();
    std::cout << programmingPortName << std::endl;
}

void printTtlPort(ProgrammerSelector & selector)
{
    ProgrammerInstance instance = selector.selectProgrammer();
    std::string programmingPortName = instance.getTtlPortName();
    std::cout << programmingPortName << std::endl;
}

static void printDigitalReadings(ProgrammerSelector & selector)
{
    ProgrammerHandle handle(selector.selectProgrammer());
    ProgrammerDigitalReadings readings = handle.digitalRead();

    std::cout << "PORTA: " << std::bitset<8>(readings.portA) << std::endl;
    std::cout << "PORTB: " << std::bitset<8>(readings.portB) << std::endl;
    std::cout << "PORTC: " << std::bitset<8>(readings.portC) << std::endl;
}

// [all-settings]
static void applySettings(ProgrammerSelector & selector, const Arguments args)
{
    assert(args.settingsSpecified());

    ProgrammerHandle handle(selector.selectProgrammer());

    if (args.restoreDefaults)
    {
        handle.restoreDefaults();
    }

    ProgrammerSettings settings = handle.getSettings();

    if (args.maxFrequencySpecified)
    {
        Programmer::setMaxFrequency(settings, args.maxFrequencyName);
    }

    if (args.frequencySpecified)
    {
        Programmer::setFrequency(settings, args.frequencyName);
    }

    if (args.regulatorModeSpecified)
    {
        settings.regulatorMode = args.regulatorMode;
    }

    if (args.vccOutputEnabledSpecified)
    {
        settings.vccOutputEnabled = args.vccOutputEnabled;
    }

    if (args.vccOutputIndicatorSpecified)
    {
        settings.vccOutputIndicator = args.vccOutputIndicator;
    }

    if (args.lineAFunctionSpecified)
    {
        settings.lineAFunction = args.lineAFunction;
    }

    if (args.lineBFunctionSpecified)
    {
        settings.lineBFunction = args.lineBFunction;
    }

    if (args.softwareVersionMajorSpecified)
    {
        settings.softwareVersionMajor = args.softwareVersionMajor;
    }

    if (args.softwareVersionMinorSpecified)
    {
        settings.softwareVersionMinor = args.softwareVersionMinor;
    }

    if (args.hardwareVersionSpecified)
    {
        settings.hardwareVersion = args.hardwareVersion;
    }

    if (args.vccVddMaxRangeSpecified)
    {
        settings.vccVddMaxRange = args.vccVddMaxRange;
    }

    if (args.vcc3v3MinSpecified)
    {
        settings.vcc3v3Min = args.vcc3v3Min;
    }

    if (args.vcc3v3MaxSpecified)
    {
        settings.vcc3v3Max = args.vcc3v3Max;
    }

    if (args.vcc5vMinSpecified)
    {
        settings.vcc5vMin = args.vcc5vMin;
    }

    if (args.vcc5vMaxSpecified)
    {
        settings.vcc5vMax = args.vcc5vMax;
    }

    handle.applySettings(settings);
}

static void run(int argc, char ** argv)
{
    Arguments args = parseArgs(argc, argv);
    adjustArguments(args);

    if (args.showHelp)
    {
        std::cout << help;
        return;
    }

    ProgrammerSelector selector;
    if (args.serialNumberSpecified)
    {
        selector.specifySerialNumber(args.serialNumber);
    }

    if (args.showList)
    {
        printProgrammerList(selector);
        return;
    }

    if (args.settingsSpecified())
    {
        applySettings(selector, args);
    }

    if (args.showStatus)
    {
        printProgrammerStatus(selector);
    }

    if (args.printProgrammingPort)
    {
        printProgrammingPort(selector);
    }

    if (args.printTtlPort)
    {
        printTtlPort(selector);
    }

    if (args.digitalRead)
    {
        printDigitalReadings(selector);
    }
}

int main(int argc, char ** argv)
{
    int exitCode = 0;

    try
    {
        run(argc, argv);
    }
    catch(const ExceptionWithExitCode & error)
    {
        std::cerr << "Error: " << error.what() << std::endl;
        exitCode = error.getCode();
    }
    catch(const std::exception & error)
    {
        std::cerr << "Error: " << error.what() << std::endl;
        exitCode = PAVRPGM_ERROR_OPERATION_FAILED;
    }

    if (exitCode == PAVRPGM_ERROR_DEVICE_NOT_FOUND)
    {
        if (pgm03aPresent())
        {
            std::cerr << std::endl;
            std::cerr << pgm03aMessage << std::endl;
        }
    }

    return exitCode;
}
