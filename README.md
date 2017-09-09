# Tic Stepper Motor Controller software

Version: 1.3.0<br>
Release date: 2017-08-29<br>
[www.pololu.com](https://www.pololu.com/)

This repository contains the source code of the configuration and control software for
the [Tic Stepper Motor Controller](https://www.pololu.com/category/212/tic-stepper-motor-controllers).
There are two programs: the Pololu Tic Command-line Utility
(ticcmd) and the Pololu Tic Control Center (ticgui).

## Installation

Installers will be available for download from the
[Tic Stepper Motor Controller User's Guide](http://www.pololu.com/docs/0J67).

See [BUILDING.md](BUILDING.md) for information about how to compile the code
from source.

## Version history

- 1.3.0 (2017-09-08):
  - Added support for building installers for Linux.
  - ticgui: Added the compact layout.  Can be enabled with `TICGUI_COMPACT=Y`.
  - ticgui: Center the window at startup.  Can be disabled with `TICGUI_CENTER=N`.
- 1.2.0 (2017-08-28):
  - ticgui: Added support for the "Serial response delay" setting.
  - ticgui: Fixed how error messages at program startup are handled.  They are now shown after the window is displayed, which fixes a bug that caused the windows to start in the upper left corner with its title bar off the screen.
  - ticgui: Improved the pin configuration interface by hiding or disabling checkboxes appropriately, and labeling pins that are permanently pulled up or down.
  - ticgui: Fixed the lower bound for "Encoder prescaler" and "Encoder postscaler" inputs.
  - ticcmd: Added `--pause` and `--pause-on-error`.
  - libpololu-tic: Added `tic_device_get_product()`.
  - Renamed the "Kill switch" error bit to "Kill switch active" everywhere.
  - CMake: install libpololu-tic, its headers, and a .pc file to the system.
  - Nix: Added support for 32-bit Linux and Raspberry Pi.
  - Fixed some compilation issues for macOS and GCC 4.9.
- 1.1.0 (2017-08-02):
  - ticgui: Added support for firmware upgrades.
  - ticgui: Improved some of the text.
  - ticcmd: Allowed '#' at the beginning of the serial number argument.
  - libpololu-tic: Added `tic_start_bootloader()`.
  - libpololu-tic and ticcmd: Added support for the "Serial response delay" setting.
- 1.0.0 (2017-07-18): Original release.