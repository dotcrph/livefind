#include "flags.hpp"
#include "funcs.hpp"

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace flags {
    const std::unordered_map<std::string, FlagType> flag_hashmap = {
        { "-v", FlagType::VERBOSITY_LVL },
        { "--verbosity", FlagType::VERBOSITY_LVL },
        { "-d", FlagType::MAX_DEPTH },
        { "--depth", FlagType::MAX_DEPTH },
        { "--max-depth", FlagType::MAX_DEPTH },
        { "-f", FlagType::FORCE_MODE },
        { "--force", FlagType::FORCE_MODE },
    };

    const std::unordered_map<std::string, int> verbosity_args = {
        { "0", 0 },
        { "silent", 0 },
        { "1", 1 },
        { "error", 1 },
        { "errors", 1 },
        { "2", 2 },
        { "warning", 2 },
        { "warnings", 2 },
        { "important", 2 },
        { "3", 3 },
        { "verbose", 3 },
        { "all", 3 },
    };

    const std::unordered_map<std::string, int> force_mode_args = {
        { "0", 0 },
        { "noforce", 0 },
        { "1", 1 },
        { "cancel", 1 },
        { "2", 2 },
        { "continue", 2 },
        { "allow", 2 },
    };

    int verbosity_lvl = 2;
    int max_depth = 1;
    int force_mode = 0;

    void parse_flag(const FlagType flag, size_t &i, 
                    int &argc, char **(&argv))
    {
        std::string arg{argv[i]};

        switch (flag) {
            case FlagType::VERBOSITY_LVL:
                if (i + 1 >= argc) {
                    log::warning("No argument provided after %s, ignoring", 
                                  arg.c_str());
                    break;
                }

                arg = argv[++i];

                if (verbosity_args.find(arg) == verbosity_args.end()) {
                    log::warning("Invalid argument \"%s\" to -v, ignoring", 
                                    arg.c_str());
                    break;
                }

                flags::verbosity_lvl = verbosity_args.at(arg);
                break;

            case FlagType::MAX_DEPTH:
                if (i + 1 >= argc) {
                    log::warning("No argument provided after %s, ignoring", 
                                  arg.c_str());
                    break;
                }

                arg = argv[++i];

                try {
                    flags::max_depth = conversions::str_to_int(arg);
                } catch (const std::invalid_argument &e) {
                    log::warning("Invalid argument \"%s\" to -d, ignoring (%s)", 
                                arg.c_str(), e.what());
                } catch (const std::out_of_range &e) {
                    log::warning("Invalid argument \"%s\" to -d, ignoring (%s)", 
                                arg.c_str(), e.what());
                }

                break;

            case FlagType::FORCE_MODE:
                if (i + 1 >= argc) {
                    log::warning("No argument provided after %s, ignoring", 
                                  arg.c_str());
                    break;
                }

                arg = argv[++i];

                if (force_mode_args.find(arg) == force_mode_args.end()) {
                    log::warning("Invalid argument \"%s\" to -f, ignoring", 
                                    arg.c_str());
                    break;
                }

                flags::force_mode = force_mode_args.at(arg);
                break;
        }
    }
}

