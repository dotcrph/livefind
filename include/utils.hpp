#pragma once

#include <string>
#include <vector>

#include <ncurses.h>
#include <menu.h>
#include <form.h>

namespace log {
    void error(const char *msg, ...);
    void warning(const char *msg, ...);
    void verbose(const char *msg, ...);
    void print_help();
}

namespace utils {
    bool try_push_dir(const std::string &dir, 
                      std::vector<std::string> *target_vec);
    void trim_whitespace(std::string &str);
}

namespace conversions {
    std::vector<std::string> convert_args(int argc, char **argv);
    int str_to_int(const std::string &in);

    constexpr int ctrl(const int ch) {
        // Convert char to ctrl+char
        return ch & 0x1f;
    }
}

namespace tui::components {
    void draw_border(WINDOW *window_ptr, const std::string &title = "");
    WINDOW *create_derwin(WINDOW *parent_win, 
                          int offset_h, int offset_w, 
                          int offset_y, int offset_x);
    void destroy_window(WINDOW *window_ptr);

    MENU *create_menu(WINDOW *parent_win, WINDOW *sub_win, ITEM **items);
    void destroy_menu(MENU *menu_ptr);

    FORM *create_form(WINDOW *parent_win, WINDOW *sub_win, FIELD **fields);
    void destroy_form(FORM *form_ptr);
}

