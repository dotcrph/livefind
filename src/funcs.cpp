#include "funcs.hpp"

#include <cstdio>
#include <cstdarg>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>

#include <ncurses.h>
#include <menu.h>
#include <form.h>

#include "flags.hpp"

namespace fs = std::filesystem;

template <typename T>
using Uptr = std::unique_ptr<T>;

namespace log {
    void error(const char *msg, ...)
    {
        if (flags::verbosity_lvl < 1) return;

        std::cout << "\033[1;31m" << "ERROR" << "\033[0m" << ": ";

        va_list args;
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);

        std::cout << std::endl;
    }

    void warning(const char *msg, ...)
    {
        if (flags::verbosity_lvl < 2) return;

        std::cout << "\033[1;33m" << "Warning" << "\033[0m" << ": ";

        va_list args;
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);

        std::cout << std::endl;
    }

    void verbose(const char *msg, ...) 
    {
        if (flags::verbosity_lvl < 3) return;

        std::cout << "\033[1;36m" << "Info" << "\033[0m" << ": ";

        va_list args;
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);

        std::cout << std::endl;
    }

    void print_help() 
    {
        std::cout <<

"\033[1mlivefind [-Options] [Directories]\033[0m\n"
"\n"
"NOTE: Livefind recognizes flags as anything listed below, even if it is in a\n"
"middle of a list of directories, and flags are also applied as encountered.\n"
"As such, you should pass flags first to avoid strange behaviour, although\n"
"you can do some cool stuff with this if you know what you are doing.\n"
"\n"
"\033[1mOptions\033[0m:\n"
"    -v --verbosity\n"
"        0 silent                      : Disable all output\n"
"        1 error errors                : Print errors only\n"
"        \033[4m2 warning warnings important\033[0m  : Print warnings & errors only\n"
"        3 verbose all                 : Print warnings, errors & info\n"
"\n"
"    -f --force\n"
"        \033[4m0 noforce\033[0m                     : Ask for risky actions\n"
"        1 cancel                      : Always cancel risky actions\n"
"        2 continue allow              : Always allow risky actions\n"
"\n"
"    -d --depth --max-depth\n"
"        [unsigned int]                : Max recursion depth\n"
"        ( Default value: 1 )\n"

        << std::endl;
    }
}

namespace utils {
    bool try_push_dir(const std::string &dir, 
                      std::vector<std::string> *target_vec) 
    {
        try {
            if (!(fs::exists(dir) && fs::is_directory(dir))) {
                log::warning("Nonexistent directory \"%s\"", dir.c_str());
                return false;
            }

            target_vec->push_back(dir);

            return true;
        } catch (const fs::filesystem_error &e) {
            log::error("( %s ) FILESYSTEM: %s", dir.c_str(), e.what());
            return false;
        }
    }

    void trim_whitespace(std::string &str)
    {
        size_t last_chr = str.find_last_not_of(" \t\n");

        if (last_chr == std::string::npos) {
            str.clear();
        } else {
            str.erase(last_chr + 1);
        }
    }

    bool quick_write_file(const std::string &msg)
    {
        auto file = std::make_unique<std::ofstream>("#LivefindOutput#");

        if (!file->is_open()) {
            log::error("Failed to make a temporary file!");
            return false;
        }

        *file << msg;

        file->close();
        return true;
    }
}

namespace conversions {
    int str_to_int(const std::string &in)
    {
        // IMPORTANT: THIS MUST ONLY BE USED INSIDE TRY/CATCH

        if (!std::regex_match(in, std::regex("^[0-9]+$"))) {
            // This check is needed because stoi does not throw 
            // an exception when it encouters a character that is 
            // not numeric (for whatever reason); this also 
            // discards negative numbers

            if (in.at(0) == '-') {
                throw std::out_of_range("Argument must be positive");
            } else if (in.at(0) == '+') {
                throw std::invalid_argument("You do not need to explicitly specify that a value is positive, you know");
            }

            throw std::invalid_argument("Argument must be numeric");
        }

        return std::stoi(in);
    }
}

namespace tui::components {
    void draw_border(WINDOW *window_ptr, const std::string &title)
    {
        static constexpr short TITLE_X_OFFSET = 2;

        box(window_ptr, 0, 0);

        if (!title.empty()) {
            wattron(window_ptr, A_REVERSE);

            mvwaddch(window_ptr, 0, TITLE_X_OFFSET, ' ');
            mvwprintw(window_ptr, 0, TITLE_X_OFFSET + 1, 
                      "%s", title.c_str());
            mvwaddch(window_ptr, 0, TITLE_X_OFFSET + title.length() + 1, 
                     ' ');

            wattroff(window_ptr, A_REVERSE);
        }
    }

    WINDOW *create_derwin(WINDOW *parent_win, 
            int offset_h, int offset_w, 
            int offset_y, int offset_x)
    {
        int parent_h, parent_w;
        getmaxyx(parent_win, parent_h, parent_w);

        return derwin(
            parent_win, 
            parent_h - offset_h, parent_w - offset_w, 
            offset_y, offset_x
        );
    }

    void destroy_window(WINDOW *window_ptr)
    {
        wborder(window_ptr, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        wrefresh(window_ptr);
        delwin(window_ptr);
    }

    MENU *create_menu(WINDOW *parent_win, WINDOW *sub_win, ITEM **items)
    {
        MENU *menu_ptr = new_menu(items);

        int parent_h, parent_w;
        getmaxyx(parent_win, parent_h, parent_w);

        set_menu_win(menu_ptr, parent_win);
        set_menu_sub(menu_ptr, sub_win);
        set_menu_format(menu_ptr, parent_h-2, 1);
        set_menu_mark(menu_ptr, " > ");

        post_menu(menu_ptr);
        return menu_ptr;
    }

    void destroy_menu(MENU *menu_ptr)
    {
        unpost_menu(menu_ptr);
        free_menu(menu_ptr);
    }

    FORM *create_form(WINDOW *parent_win, WINDOW *sub_win, FIELD **fields)
    {
        FORM *form_ptr = new_form(fields);

        int parent_h, parent_w;
        getmaxyx(parent_win, parent_h, parent_w);

        set_form_win(form_ptr, parent_win);
        set_form_sub(form_ptr, sub_win);

        post_form(form_ptr);
        return form_ptr;
    }

    void destroy_form(FORM *form_ptr)
    {
        unpost_form(form_ptr);
        free_form(form_ptr);
    }
}

