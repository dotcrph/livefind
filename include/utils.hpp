#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include <ncurses.h>
#include <menu.h>
#include <form.h>

namespace log {
    void error(const char *fmt, ...);
    /// Description:
    ///     A variadic wrapper around vfprintf. Intended for logging error 
    ///     messages.

    void warning(const char *fmt, ...);
    /// Description:
    ///     A variadic wrapper around vfprintf. Intended for logging warnings.

    void verbose(const char *fmt, ...);
    /// Description:
    ///     A variadic wrapper around vfprintf. Intended for logging 
    ///     debugging messages and other information that *might* be 
    ///     useful.

    void logGeneric(
        int minVerbosityLevel,
        FILE *outputBuffer,
        const char *prefix,
        const char *fmt, 
        va_list args);
    /// Description:
    ///     A generic (as in general) logging function that serves as a base 
    ///     for other public logging functions.

    void printHelp();
    /// Description:
    ///     Prints help message
}

namespace utils {
    bool isPathOK(const std::filesystem::path &path);
    /// Description:
    ///     Checks if path is a directory that exists 
    /// 
    /// Returns:
    ///     True if path exists and is a directory, else false

    void strTrimWhitespace(std::string &str);
    /// Description:
    ///     Trims trailing whitespace in str; modifies original string.

    void strToLower(std::string &str);
    /// Description:
    ///     Changes all letters to lowercase in str; modifies original string.
}

namespace conversions {
    std::vector<std::string> convertArgs(int argc, char **argv);
    /// Description:
    ///     Converts CLI arguments from an array of char pointers to a vector 
    ///     of strings.
    ///
    /// Returns:
    ///     Vector of strings from the original char *argv[argc].

    int tryStrToPositiveInt(const std::string &in, int &out);
    /// Description:
    ///     Tries to convert in string to a positive signed integer out. 
    ///     Returns 0 on success or one of the following exit codes on 
    ///     failure:
    ///         1: String contains something other than digits
    ///         2: String contains a number that does not fit in an int
    ///
    /// Returns:
    ///     0 on success (and sets out reference to converted value), else one 
    ///     of the error codes

    constexpr int ctrl(const int ch)
    /// Description: 
    ///     Converts int character code to Ctrl+character code
    ///
    /// Returns:
    ///     Int code that corresponds to Ctrl+ch combo
    {
        return ch & 0x1f;
    }
}

namespace types {
    class Entry final
    /// Description:
    ///     A class that contains a single ncurses' ITEM pointer with 
    ///     associated metadata (currently only displayed string) and 
    ///     automatically manages this ITEM's lifetime. This class is 
    ///     necessary because ncurses' new_item() does not copy its 
    ///     arguments.
    {
    private:
        std::string name_;
        ITEM *item_;

    public:
        Entry(std::string name);
        Entry(const Entry &other);
        Entry &operator=(const Entry &other);
        Entry(Entry &&other) noexcept;
        Entry &operator=(Entry &&other) noexcept;
        ~Entry();

        const std::string &name() const { return name_; }
        ITEM *item() { return item_; }

        const ITEM *initialize();
        /// Description:
        ///     Calls ncurses' new_item with name_.c_str() and nullptr as 
        ///     arguments. Should only be used once as it does not release 
        ///     previously allocated memory!
        ///
        /// Returns:
        ///     An ITEM * to allocated item.
    };
}

