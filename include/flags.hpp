#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>

namespace flags {
    enum class FlagType {
        VERBOSITY_LVL,
        MAX_DEPTH,
        FORCE_MODE
    };

    extern const std::unordered_map<std::string, FlagType> flag_hashmap;
    extern const std::unordered_map<std::string, int> verbosity_args;
    extern const std::unordered_map<std::string, int> force_mode_args;

    extern int verbosity_lvl;
    extern int max_depth;
    extern int force_mode;

    void parse_flag(const FlagType flag, size_t &i, int &argc, char **(&argv));
}

