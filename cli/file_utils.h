// Helpers for opening input and output files.  If the file cannot be opened, an
// exception is thrown.  If the filname is "-", the standard input or output is
// used instead of actually opening a file.

#pragma once

#include <memory>
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>

namespace
{
  std::shared_ptr<std::istream> open_file_or_pipe_input(std::string filename)
  {
    std::shared_ptr<std::istream> file;
    if (filename == "-")
    {
      file.reset(&std::cin, [](std::istream *){});
    }
    else
    {
      std::ifstream * disk_file = new std::ifstream();
      file.reset(disk_file);
      disk_file->open(filename);
      if (!*disk_file)
      {
        int error_code = errno;
        throw std::runtime_error(filename + ": " + strerror(error_code) + ".");
      }
    }
    return file;
  }

  std::shared_ptr<std::ostream> open_file_or_pipe_output(std::string filename)
  {
    std::shared_ptr<std::ostream> file;
    if (filename == "-")
    {
      file.reset(&std::cout, [](std::ostream *){});
    }
    else
    {
      std::ofstream * disk_file = new std::ofstream();
      file.reset(disk_file);
      disk_file->open(filename);
      if (!*disk_file)
      {
        int error_code = errno;
        throw std::runtime_error(filename + ": " + strerror(error_code) + ".");
      }
    }
    return file;
  }
}
