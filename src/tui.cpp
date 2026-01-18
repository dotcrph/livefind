#include "tui.hpp"

#include <menu.h>
#include <vector>

#include <ncurses.h>
#include <form.h>

#include "funcs.hpp"
#include "main.hpp"

namespace tui {
    constexpr int CTRLC = 3;
    constexpr int CTRLQ = 17;

    constexpr int CTRLN = 14;
    constexpr int CTRLP = 16;

    std::vector<ITEM*> *matched_dirs = new std::vector<ITEM*>();

    WINDOW *files_window;
    WINDOW *input_window;

    MENU *files_menu;

    FORM *input_form;
    FIELD *input_fields[2];

    void run() {
        initscr();

        noecho();
        cbreak();
        raw();
        keypad(stdscr, TRUE); // For arrow keys

        curs_set(0);
        refresh(); // A refresh is required to draw windows

        files_window = newwin(LINES - 3, COLS, 0, 0);
        components::draw_border(files_window, "LiveFind");
        wrefresh(files_window);

        input_window = newwin(3, COLS, LINES - 3, 0);
        components::draw_border(input_window, "Input");
        wrefresh(input_window);

        for (const auto &path_str : *paths) {
            matched_dirs->push_back(new_item(path_str.c_str(), nullptr));
        }
        matched_dirs->push_back(nullptr);

        files_menu = components::create_menu(files_window, 
                                             matched_dirs->data());

        input_fields[0] = new_field(1, COLS-2, 0, 0, 0, 0);
        input_fields[1] = nullptr;

        input_form = components::create_form(input_window, input_fields);

        move(LINES - 2, 1);
        curs_set(1);

        while (process_input(getch()));

        components::destroy_window(files_window);
        components::destroy_window(input_window);

        components::destroy_menu(files_menu);

        for (auto &item : *matched_dirs) {
            free_item(item);
        }

        delete matched_dirs;

        components::destroy_form(input_form);

        for (auto &field : input_fields) {
            free_field(field);
        }

        endwin();
    }

    void update_matched_dirs(const std::string &substring) {
        components::destroy_menu(files_menu);

        for (auto &item : *matched_dirs) {
            free_item(item);
        }

        wclear(files_window);

        matched_dirs->clear();

        for (const auto &path : *paths) {
            if (substring.empty() 
                || path.find(substring) != std::string::npos) {
                matched_dirs->push_back(new_item(path.c_str(), nullptr));
            }
        }
        matched_dirs->push_back(nullptr);

        files_menu = components::create_menu(files_window, 
                                             matched_dirs->data());

        components::draw_border(files_window, "LiveFind");
        wrefresh(files_window);
        wrefresh(input_window);
    }

    bool process_input(const int ch) 
    {
        switch (ch) {
            case '\n':
            case KEY_ENTER:
                out_path = item_name(current_item(files_menu));
                utils::trim_whitespace(out_path);
                return false;

            case CTRLC:
            case CTRLQ:
                return false;

            case CTRLN:
            case KEY_DOWN:
                menu_driver(files_menu, REQ_NEXT_MATCH);
                wrefresh(files_window);
                break;

            case CTRLP:
            case KEY_UP:
                menu_driver(files_menu, REQ_PREV_MATCH);
                wrefresh(files_window);
                break;

            case KEY_BACKSPACE:
                form_driver(input_form, REQ_PREV_CHAR);
                form_driver(input_form, REQ_DEL_CHAR);
            default:
                form_driver(input_form, ch);
                form_driver(input_form, REQ_VALIDATION);

                std::string input{field_buffer(input_fields[0], 0)};
                utils::trim_whitespace(input);
                update_matched_dirs(input);

                move(LINES - 2, input.length() + 1);
                break;
        }

        return true;
    }
}
