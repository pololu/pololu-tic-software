#pragma once

#include <tic.hpp>
#include "config.h"
#include "arg_reader.h"
#include "device_selector.h"
#include "exit_codes.h"
#include "exception_with_exit_code.h"
#include "file_utils.h"

#include <algorithm>
#include <bitset>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <thread>

void print_status(const tic::variables & vars,
  const std::string & name,
  const std::string & serial_number,
  const std::string & firmware_version);
