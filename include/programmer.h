// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/** Main header file for the C++ library libpavrpgm.
 *
 * This library supports communciating with the Pololu USB AVR Programmer v2.
 *
 * This is only intended to be used as a static library by the other projects in
 * this repository.  The interface described in this header is considered to be
 * an internal implementation detail, and it may change at any time.
 */

#pragma once

#include <libusbp.hpp>
#include <vector>
#include <string>
#include <cstdint>

#include "pgm04a_native_usb_protocol.h"
#include "programmer_frequency_tables.h"

/** The maximum firmware major version supported by this program.  If we make a
 * breaking change in the firmware, we can use this to be sure that old software
 * will not try to talk to new firmware and mess something up. */
#define PGM04A_FIRMWARE_VERSION_MAJOR_MAX 1

struct ProgrammerSettings;
struct ProgrammerVariables;

class Programmer
{
public:
    Programmer() = delete;

    // Gets the name (string with a number of kHz) of the maximum ISP frequency.
    // This is only used by the programmer if SCK_DURATION is 0.
    static std::string getMaxFrequencyName(uint32_t ispFastestPeriod);

    // Sets the maximum frequency by its name.  The name must exactly match
    // one of the allowed values.
    static void setMaxFrequency(ProgrammerSettings &, std::string maxFrequencyName);

    // Gets the name (string with a number of kHz) of the frequency actually
    // being used by the programmer, which depends on two parameters:
    // SCK_DURATION and ISP_FASTEST_PERIOD.
    static std::string getFrequencyName(uint32_t sckDuration, uint32_t ispFastestPeriod);

    // Sets the actual frequency being used by the programmer by its name.  The
    // name must exactly match one of the allowed values.  This function always
    // sets SCK_DURATION, and it only sets ISP_FASTEST_PERIOD if necessary.
    static void setFrequency(ProgrammerSettings &, std::string frequencyName);

    static std::string convertProgrammingErrorToShortString(uint8_t programmingError);

    // This long string does not stand alone (e.g. it could be empty if there was
    // no error or an unknown error).  It should also be displayed below the
    // output of convertProgrammingErrorToShortString.
    static std::string convertProgrammingErrorToLongString(uint8_t programmingError);

    static std::string convertDeviceResetToString(uint8_t deviceReset);

    static std::string convertRegulatorModeToString(uint8_t regulatorMode);

    static std::string convertRegulatorLevelToString(uint8_t regulatorLevel);

    static std::string convertLineFunctionToString(uint8_t lineFunction);
};

class ProgrammerInstance
{
public:
    ProgrammerInstance();
    explicit ProgrammerInstance(
        libusbp::device, libusbp::generic_interface,
        std::string serialNumber, uint16_t firmwareVersion);

    static std::string getName();

    // An operating system-specific identifier for this device.
    std::string getOsId() const;

    std::string getSerialNumber() const;
    uint16_t getFirmwareVersion() const;

    // Returns a string like "1.03", not including any firmware modification codes.
    // See ProgrammerHandle::getFirmwareVersionString().
    std::string getFirmwareVersionString() const;

    uint8_t getFirmwareVersionMajor() const;

    uint8_t getFirmwareVersionMinor() const;

    std::string getProgrammingPortName() const;
    std::string getTtlPortName() const;

    /** Same as getProgrammingPortName but it returns "(unknown)" instead of
     * throwing exceptions from the USB library. */
    std::string tryGetProgrammingPortName() const;

    /** Same as getTtlPortName but it returns "(unknown)" instead of throwing
     * exceptions from the USB library. */
    std::string tryGetTtlPortName() const;

    operator bool() const;

    libusbp::device usbDevice;
    libusbp::generic_interface usbInterface;

private:
    std::string serialNumber;
    uint16_t firmwareVersion;
};

std::vector<ProgrammerInstance> programmerGetList();

// [all-settings]
struct ProgrammerSettings
{
    uint32_t sckDuration = 0;
    uint32_t ispFastestPeriod = 0;
    uint8_t regulatorMode = 0;
    bool vccOutputEnabled = 0;
    uint8_t vccOutputIndicator = 0;
    uint8_t lineAFunction = 0;
    uint8_t lineBFunction = 0;
    uint32_t vccVddMaxRange = 0;  // units: mV
    uint32_t vcc3v3Min = 0;  // units: mV
    uint32_t vcc3v3Max = 0;  // units: mV
    uint32_t vcc5vMin = 0;  // units: mV
    uint32_t vcc5vMax = 0;  // units: mV
    uint32_t hardwareVersion = 0;
    uint32_t softwareVersionMajor = 0;
    uint32_t softwareVersionMinor = 0;
};

struct ProgrammerVariables
{
    uint8_t lastDeviceReset = 0;
    bool hasResultsFromLastProgramming = 0;
    uint8_t programmingError = 0;
    uint16_t targetVccMeasuredMinMv = 0;
    uint16_t targetVccMeasuredMaxMv = 0;
    uint16_t programmerVddMeasuredMinMv = 0;
    uint16_t programmerVddMeasuredMaxMv = 0;
    uint16_t targetVccMv = 0;
    uint16_t programmerVddMv = 0;
    uint8_t regulatorLevel = 0;
    bool inProgrammingMode = 0;
};

struct ProgrammerDigitalReadings
{
    uint8_t portA;
    uint8_t portB;
    uint8_t portC;
};

class ProgrammerHandle
{
public:
    ProgrammerHandle();
    explicit ProgrammerHandle(ProgrammerInstance);

    void close();

    const ProgrammerInstance & getInstance() const;

    operator bool() const
    {
        return handle;
    }

    // Returns the firmware version string, including any modification
    // codes (e.g. "1.07nc").
    std::string getFirmwareVersionString();

    ProgrammerSettings getSettings();
    void validateSettings(const ProgrammerSettings &);
    void applySettings(const ProgrammerSettings &);
    void restoreDefaults();

    ProgrammerVariables getVariables();

    ProgrammerDigitalReadings digitalRead();

private:
    uint8_t getRawSetting(uint8_t id);
    void setRawSetting(uint8_t id, uint8_t value);
    uint8_t getRawVariable(uint8_t id);

    std::string cachedFirmwareVersion;
    libusbp::generic_handle handle;
    ProgrammerInstance instance;
};

// Returns true if a Pololu USB AVR Programmer (pgm03a) is connected
// to the computer.
bool pgm03aPresent();
extern const char * pgm03aMessage;
