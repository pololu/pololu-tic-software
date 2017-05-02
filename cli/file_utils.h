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

  void write_string_to_file(std::string filename, std::string contents)
  {
    auto stream = open_file_or_pipe_output(filename);
    *stream << contents;
    if (stream->fail())
    {
      // TODO: can we get more details about what failed?
      throw std::runtime_error("Failed to write to file or pipe.");
    }
  }

  std::string read_string_from_file(std::string filename)
  {
    auto stream = open_file_or_pipe_input(filename);
    std::string contents;
    *stream >> contents;
    if (stream->fail())
    {
      throw std::runtime_error("Failed to read from file or pipe.");
    }
    return contents;
  }
}
