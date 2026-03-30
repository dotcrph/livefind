#include "flags.hpp"
#include "utils.hpp"

#include <string>
#include <unordered_map>

namespace flags {
    int verbosityLvl = 2;
    int maxDepth = 1;
    int forceMode = 0;

    bool tryParseAsFlag(size_t &i, const std::vector<std::string> &args)
    {
        using parser_func_ptr 
            = void (*)(size_t &, const std::vector<std::string> &);

        static std::unordered_map<std::string, parser_func_ptr> 
        flagParsers = {
            { "-v",             parseVerbosityLvl },
            { "--verbosity",    parseVerbosityLvl },

            { "-d",             parseMaxDepth },
            { "--depth",        parseMaxDepth },
            { "--max-depth",    parseMaxDepth },

            { "-f",             parseForceMode },
            { "--force",        parseForceMode },
        };

        if (flagParsers.find(args[i]) == flagParsers.end())
            return false;

        flagParsers[args[i]](i, args);
        return true;
    }

    void parseVerbosityLvl(size_t &i, const std::vector<std::string> &args)
    {
        static std::unordered_map<std::string, int> verbosityArgs = {
            { "0",          0 },
            { "silent",     0 },

            { "1",          1 },
            { "error",      1 },
            { "errors",     1 },

            { "2",          2 },
            { "warning",    2 },
            { "warnings",   2 },
            { "important",  2 },

            { "3",          3 },
            { "verbose",    3 },
            { "all",        3 },
        };

        const std::string &arg = args[i];

        if (i + 1 >= args.size()) {
            log::warning("No argument provided after '%s', ignoring", 
                         arg.c_str());
            return;
        }

        const std::string &value = args[++i];

        if (!verbosityArgs.contains(value)) {
            log::warning("Invalid argument '%s' to '%s', ignoring", 
                         value.c_str(), arg.c_str());
            return;
        }

        flags::verbosityLvl = verbosityArgs[value];
    }

    void parseMaxDepth(size_t &i, const std::vector<std::string> &args)
    {
        const std::string &arg = args[i];

        if (i + 1 >= args.size()) {
            log::warning("No argument provided after '%s', ignoring", 
                         arg.c_str());
            return;
        }

        const std::string &value = args[++i];

        if (conversions::tryStrToPositiveInt(value, flags::maxDepth) == 0)
            return;

        log::warning("Invalid argument '%s' to '%s', ignoring", value.c_str(), 
                                                                arg.c_str());
    }

    void parseForceMode(size_t &i, const std::vector<std::string> &args)
    {
        static std::unordered_map<std::string, int> forceModeArgs = {
            { "0",          0 },
            { "noforce",    0 },

            { "1",          1 },
            { "cancel",     1 },

            { "2",          2 },
            { "continue",   2 },
            { "allow",      2 },
        };

        const std::string &arg = args[i];

        if (i + 1 >= args.size()) {
            log::warning("No argument provided after '%s', ignoring", 
                          arg.c_str());
            return;
        }

        const std::string &value = args[++i];

        if (!forceModeArgs.contains(value)) {
            log::warning("Invalid argument '%s' to '%s', ignoring", 
                         value.c_str(), arg.c_str());
            return;
        }

        flags::forceMode = forceModeArgs[value];
    }
}

