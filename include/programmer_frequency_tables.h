#pragma once

#include <cstdint>
#include <vector>

struct ProgrammerFrequency
{
    // The period in raw units (twelfths of a microsecond).
    uint16_t period;

    // The string representation of this period (in units of kHz but without any
    // suffix in the string).
    const char * name;
};

/** This table defines how the meaning of the SCK_DURATION parameter of the
 * programmer's firmware works.
 *
 * If SCK_DURATION is 1 or more, the programmer (using a table similar to this
 * one) uses an ISP frequency that is obtained by treating the value of the
 * SCK_DURATION parameter as an index and looking it up in this table.
 *
 * If SCK_DURATION is 0, this table is ignored and the programmer uses the
 * ISP_FASTEST_PERIOD parameter instead.  Element 0 of this table is a dummy
 * element that we set equal to the default frequency specified by
 * ISP_FASTEST_PERIOD.
 *
 * These values are not arbitrary: they were picked so that the frequency
 * returned by this table will be as high as possible, while (usually) not
 * exceeding the frequency used by the STK500 for the same SCK_DURATION byte.
 * This means our frequency will work for just as many AVRs, and someone can
 * confidently use the UI in Atmel Studio or AVRDUDE without worrying that they
 * are picking a frequency that is too fast.
 *
 * HOWEVER, an exception to the rule above is that we cannot have frequencies
 * below 1.465 kHz, so any frequency below that gets rounded.
 */
extern const std::vector<ProgrammerFrequency> programmerStk500FrequencyTable;

/** This table defines the meaning of the ISP_FASTEST_PERIOD parameter of the
 * programmer's firmware.
 *
 * If SCK_DURATION is 0, the programmer will use the ISP frequency that is
 * obtained by treating the value of ISP_FASTEST_PERIOD as an index and looking
 * it up in this table.  Except for the first two elements, the frequencies in
 * this table follow the formula:
 *
 *    frequency = (12 MHz) / index
 */
extern const std::vector<ProgrammerFrequency> programmerFullMaxFrequencyTable;

/** This table holds the set of frequencies that the software allows you to set
 * for the "Max ISP Frequency" setting, which corresponds to the
 * ISP_FASTEST_PERIOD parameter.
 *
 * This table could also be computed by selecting the elements from
 * programmerFullMaxFrequencyTable that have a period between
 * PGM04A_ISP_FASTEST_PERIOD_MIN and PGM04A_ISP_FASTEST_PERIOD_MAX. */
extern const std::vector<ProgrammerFrequency> programmerAllowedMaxFrequencyTable;

/** This table holds a useful subset of programmerAllowedMaxFrequencyTable.
 * This allows us to show a nice selection of choices in a user interface
 * without overwhelming the user with too many choices.
 *
 * This table could also be computed by taking all the frequencies in both
 * programmerSuggestedFrequencyTable and programmerAllowedMaxFrequencyTable. */
extern const std::vector<ProgrammerFrequency> programmerSuggestedMaxFrequencyTable;

/** This table holds the set of frequencies that the software allows you to set
 * for the "ISP Frequency" setting, which is related to two firmware parameters:
 * SCK_DURATION and ISP_FASTEST_PERIOD.
 *
 * This table could have also been computed by removing the first element of
 * programmerStk500FrequencyTable, prepending the table with
 * programmerAllowedMaxFrequencyTable, removing the duplicates at the end, and
 * sorting the table in descending order by frequency. */
extern const std::vector<ProgrammerFrequency> programmerAllowedFrequencyTable;

/** This table holds a useful subset of programmerAllowedFrequencyTable.  The highest
 * frequencies are picked somewhat arbitrarily.  Many of these frequencies were
 * picked to match the user interface of Atmel Studio, which has notches for
 * theose frequencies that make them easy to select by just using the mouse. */
extern const std::vector<ProgrammerFrequency> programmerSuggestedFrequencyTable;

/** The default frequency of the firmware. */
extern const ProgrammerFrequency programmerDefaultFrequency;

/** The default max frequency of the firmware. */
extern const ProgrammerFrequency programmerDefaultMaxFrequency;
