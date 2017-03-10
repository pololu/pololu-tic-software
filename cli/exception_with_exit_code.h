#pragma once

#include <stdexcept>

class ExceptionWithExitCode : public std::exception
{
public:
    explicit ExceptionWithExitCode(uint8_t code, std::string message)
        : code(code), msg(message)
    {
    }

    virtual const char * what() const noexcept
    {
        return msg.c_str();
    }

    std::string message() const
    {
        return msg;
    }

    uint8_t getCode() const noexcept
    {
        return code;
    }

private:
    uint8_t code;
    std::string msg;
};
