#pragma once

#include <vector>
#include <string>

namespace flags {
    extern int verbosityLvl;
    /// Description:
    ///     The minimum "severity" level of stderr messages allowed, i.e. how 
    ///     important must be the information for the program to print the 
    ///     message. Allowed values are in [0-3].

    extern int maxDepth;
    /// Description:
    ///     The maximum subdirectory depth allowed. The starting directory has 
    ///     a depth of 0. Allowed values are in [0 - MAX_INT].

    extern int forceMode;
    /// Description:
    ///     Defines the behaviour of a program when tryIterateDirs() fails.
    ///     Allowed values:
    ///         0: ask user for confirmation before proceeding
    ///         1: always stop execution
    ///         2: always continue execution

    bool tryParseAsFlag(size_t &i, const std::vector<std::string> &args);
    /// Description:
    ///     Tries to parse args[i] as a flag. Returns false if args[i] is not 
    ///     a flag, else returns true.

    void parseVerbosityLvl(size_t &i, const std::vector<std::string> &args);
    /// Description:
    ///     Parses verbosityLvl. Only sets the variable if the next argument 
    ///     exists and is one of the following:
    ///         "0"           0
    ///         "silent"      0
    ///
    ///         "1"           1
    ///         "error"       1
    ///         "errors"      1
    ///
    ///         "2"           2
    ///         "warning"     2
    ///         "warnings"    2
    ///         "important"   2
    ///
    ///         "3"           3
    ///         "verbose"     3
    ///         "all"         3

    void parseMaxDepth(size_t &i, const std::vector<std::string> &args);
    /// Description:
    ///     Parses maxDepth. Only sets the variable if the next argument 
    ///     exists and is a positive integer (an integer without any sign; 
    ///     unary plus is not allowed).

    void parseForceMode(size_t &i, const std::vector<std::string> &args);
    /// Description:
    ///     Parses forceMode. Only sets the variable if the next argument 
    ///     exists and is one of the following:
    ///         "0"           0
    ///         "noforce"     0
    ///
    ///         "1"           1
    ///         "cancel"      1
    ///
    ///         "2"           2
    ///         "continue"    2
    ///         "allow"       2
}

