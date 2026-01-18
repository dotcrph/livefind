#pragma once

#include <string>

namespace tui {
    void run();
    void update_matched_dirs(const std::string &substring);
    bool process_input(const int ch);
}
