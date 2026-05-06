#include "utils.hpp"

#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdarg>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <filesystem>

#include <ncurses.h>
#include <menu.h>
#include <form.h>

#include "flags.hpp"

namespace log {
    void error(const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);

        logGeneric(
            1,          // Min verbosity lvl
            stderr,     // Output stream
            "\x1b[1;31mERROR\x1b[0m",   // Prefix
            fmt,        // Format string
            args        // Variadic arguments
        );

        va_end(args);
    }

    void warning(const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);

        logGeneric(
            2,          // Min verbosity lvl
            stderr,     // Output stream
            "\x1b[1;33mWarning\x1b[0m", // Prefix
            fmt,        // Format string
            args        // Variadic arguments
        );

        va_end(args);
    }

    void verbose(const char *fmt, ...) 
    {
        va_list args;
        va_start(args, fmt);

        logGeneric(
            3,          // Min verbosity lvl
            stderr,     // Output stream
            "\x1b[1;36mInfo\x1b[0m",    // Prefix
            fmt,        // Format string
            args        // Variadic arguments
        );

        va_end(args);
    }

    void logGeneric(
        int minVerbosityLevel,
        FILE *outputBuffer,
        const char *prefix,
        const char *fmt, 
        va_list args)
    {
        if (flags::verbosityLvl < minVerbosityLevel) 
            return;

        fprintf(outputBuffer, "%s: ", prefix);
        vfprintf(outputBuffer, fmt, args);
        putc('\n', outputBuffer);
    }

    void printHelp() 
    {
        // NOTE: I am not using a raw string here because of ANSI SGR
        std::clog <<

"\x1b[1mlivefind [-Options] [Directories]\x1b[0m\n"
"\n"
"NOTE: Livefind recognizes flags as anything listed below, even if it is in a\n"
"middle of a list of directories, and flags are also applied as encountered.\n"
"As such, you should pass flags first to avoid strange behaviour, although\n"
"you can do some cool stuff with this if you know what you are doing.\n"
"\n"
"\x1b[1mOptions\x1b[0m:\n"
"    -v --verbosity\n"
"        0 silent                      : Disable all output\n"
"        1 error errors                : Print errors only\n"
"        \x1b[4m2 warning warnings important\x1b[0m  : Print warnings & errors only\n"
"        3 verbose all                 : Print warnings, errors & info\n"
"\n"
"    -f --force\n"
"        \x1b[4m0 noforce\x1b[0m                     : Ask for risky actions\n"
"        1 cancel                      : Always cancel risky actions\n"
"        2 continue allow              : Always allow risky actions\n"
"\n"
"    -d --depth --max-depth\n"
"        [unsigned int]                : Max recursion depth\n"
"        ( Default value: 1 )\n\n"

        << std::flush;
    }
}

namespace utils {
    bool isPathOK(const std::filesystem::path &path)
    {
        std::error_code ec;

        if (std::filesystem::is_directory(path, ec))
            return true;

        if (ec)
            log::warning("( %s ) FILESYSTEM: %s", path.c_str(), 
                                                  ec.message().c_str());

        return false;
    }

    void strTrimWhitespace(std::string &str)
    {
        size_t lastNotWhitespace = str.find_last_not_of(" \t\n");

        if (lastNotWhitespace == std::string::npos)
            str.clear();
        else
            str.resize(lastNotWhitespace + 1);
    }

    void strToLower(std::string &str)
    {
        if (str.size() == 0)
            return;

        char *c = &(str[0]);
        while (*c) {
            *c = std::tolower(*c);
            c++;
        }

        // I will just leave this here
        // while (*(c++) = std::tolower(*c));
    }
}

namespace conversions {
    std::vector<std::string> convertArgs(int argc, char **argv)
    {
        std::vector<std::string> args;
        args.reserve(argc);

        for (int i = 0; i < argc; i++)
            args.emplace_back(argv[i]);

        return args;
    }

    int tryStrToPositiveInt(const std::string &in, int &out)
    {
        for (const char &c : in) {
            if (std::isdigit(c))
                continue;

            log::error("'%s' is not a positive integer!", in.c_str());
            return 1;
        }

        int result = 0;
        int power = 1;

        for (size_t i = 0; i < in.size(); i++) {
            // Reading digits from the end
            const char &digit = in[in.size() - i - 1];

            int delta = (digit - '0') * power;

            if (std::numeric_limits<int>::max() - delta < result) {
                log::error("'%s' is too big!", in.c_str());
                return 2;
            }

            result += delta;
            power *= 10;
        }

        out = result;
        return 0;
    }
}

