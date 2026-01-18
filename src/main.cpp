#include "main.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>

#include "flags.hpp"
#include "funcs.hpp"
#include "tui.hpp"

namespace fs = std::filesystem;

template <typename T>
using Uptr = std::unique_ptr<T>;

auto paths = std::make_unique<std::vector<std::string>>();
std::string out_path{};

int main(int argc, char *argv[]) 
{

    if (argc == 2 && (!strcmp(argv[1], "-h") 
                       || !strcmp(argv[1], "--help"))) {
        log::print_help();
        return EXIT_FAILURE;
    }

    if (!try_parse_args(argc, argv)) {
        return EXIT_FAILURE;
    }

    tui::run();

    if (!utils::quick_write_file(out_path)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool try_parse_args(int &argc, char **(&argv))
{
    if (argc <= 1) {
        log::warning("No arguments provided, adding current directory to index");
        return try_add_root_dir(".");
    }

    size_t i = 1;
    while (i < argc) {
        std::string arg{argv[i]};

        if (flags::flag_hashmap.find(arg) != flags::flag_hashmap.end()) {
            flags::parse_flag(flags::flag_hashmap.at(arg), i, argc, argv);
        } else {
            log::verbose("Adding directory \"%s\" to index", arg.c_str());

            if (!try_add_root_dir(arg, true)) {
                return false;
            }
        }

        i++;
    }

    if (paths->size() == 0) {
        log::warning("No paths provided, adding current directory to index");
        return try_add_root_dir(".");
    }

    return true;
}

bool try_add_root_dir(const std::string &path, const bool canIgnore) 
{
    if (!utils::try_push_dir(path, paths.get())) {
        if (canIgnore) {
            log::warning("Failed to add \"%s\" to index, ignoring",
                            path.c_str());
            return true;
        } else {
            log::error("Failed to add \"%s\" to index", path.c_str());
            return false;
        }
    }

    if (!try_iterate_dirs(paths->back())) {
        switch (flags::force_mode) {
            case 1:
                return false;
            case 2:
                return true;
            case 0:
                std::cout << "Something went wrong while iterating through directories. Do you wish to continue? (y/\033[4mn\033[0m)"
                          << std::endl;
                char input = 'n';
                std::cin >> input;
                return (input == 'y' || input == 'Y');
        }
    }

    return true;
}

bool try_iterate_dirs(const fs::path &path) 
{
    fs::recursive_directory_iterator itr(path);
    fs::recursive_directory_iterator end;

    while (itr != end) {
        if (itr.depth() > flags::max_depth) {
            std::error_code ec;
            itr.pop(ec);
            if (ec) {
                log::error("Failed to pop recursive_directory_iterator, breaking the loop (%s)",
                              ec.message().c_str());
                return false;
            }

            continue;
        }

        if (fs::is_directory(itr->path())) {
            log::verbose("Adding entry \"%s\"", itr->path().c_str());

            try {
                paths->push_back(itr->path());
            } catch (const fs::filesystem_error &e) {
                log::warning("( %s ) FILESYSTEM: %s\nIgnoring this entry", 
                                itr->path().c_str(), e.what());
            }
        }

        std::error_code ec;
        itr.increment(ec);
        if (ec) {
            log::error("Failed to increment recursive_directory_iterator, breaking the loop (%s)",
                          ec.message().c_str());
            return false;
        }
    }

    return true;
}

// void cr_res_w() {
//     g_res_w = newwin(LINES - 3, COLS, 0, 0);
//     g_res_m = new_menu(nullptr);
//     // g_res_m = new_menu(g_res_i.data());;
//
//     set_menu_win(g_res_m, g_res_w);
//     set_menu_sub(g_res_m, derwin(g_res_w, LINES-5, COLS-2, 1, 1));
//     set_menu_format(g_res_m, 5, 1);
//     set_menu_mark(g_res_m, " * ");
//
//     box(g_res_w, 0, 0);
//     print_w_title(g_res_w, dir_str);
//
//     post_menu(g_res_m);
//     wrefresh(g_res_w);
// }
//
// void cr_input_w() {
//     g_input_w = newwin(3, COLS, LINES - 3, 0);
//
//     g_input_fd[0] = new_field(
//     /*       Height */ 1,
//     /*        Width */ COLS-2,
//     /*        Y pos */ 0,
//     /*        X pos */ 0,
//     /*  Offscr rows */ 0,
//     /* Working bufs */ 0
//     );
//
//     set_field_fore(g_input_fd[0], A_BOLD);
//     field_opts_off(g_input_fd[0], O_AUTOSKIP);
//     field_opts_off(g_input_fd[0], O_STATIC);
//
//     g_input_fd[1] = NULL;
//
//     g_input_fm = new_form(g_input_fd);
//
//     set_form_win(g_input_fm, g_input_w);
//     set_form_sub(g_input_fm, derwin(g_input_w, 1, COLS - 2, 1, 1));
//
//     box(g_input_w, 0, 0);
//     print_w_title(g_input_w, std::string("Search"));
//
//     post_form(g_input_fm);
//     wrefresh(g_input_w);
// }
//
// void process_inp(const short &ch) {
//     switch (ch) {
//         case KEY_BACKSPACE:
//             form_driver(g_input_fm, REQ_PREV_CHAR);
//             form_driver(g_input_fm, REQ_DEL_CHAR);
//             break;
//
//         case KEY_LEFT:
//             form_driver(g_input_fm, REQ_PREV_CHAR);
//             break;
//
//         case KEY_RIGHT:
//             form_driver(g_input_fm, REQ_NEXT_CHAR);
//             break;
//
//         default:
//             form_driver(g_input_fm, ch);
//             break;
//     }
//
//     wrefresh(g_res_w);
//     wrefresh(g_input_w);
// }
//
