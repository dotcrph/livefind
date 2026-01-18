#include "tui.hpp"

#include <string>
#include <unordered_set>
#include <vector>

#include <ncurses.h>
#include <form.h>
#include <menu.h>

#include "funcs.hpp"
#include "main.hpp"

namespace tui {
    WINDOW *files_window;
    WINDOW *files_derwin;
    WINDOW *input_window;
    WINDOW *input_derwin;

    MENU *files_menu;
    std::vector<ITEM*> *files_items;

    FORM *input_form;
    FIELD *input_fields[2];

    void cleanup();
    void remove_files_items(const std::string &new_search);
    void add_files_items(const std::string &new_search);
    void process_input(const int ch);

    std::string run()
    {
        files_items = new std::vector<ITEM*>();

        for (const auto &path : *paths) {
            files_items->push_back(new_item(path.c_str(), nullptr));
        }
        files_items->push_back(nullptr);

        initscr();

        noecho();
        cbreak();
        raw();
        keypad(stdscr, TRUE); // For arrow keys

        curs_set(1);

        refresh(); // A refresh is required to draw windows

        ///////////////////////////////
        // Create directories window //
        ///////////////////////////////

        files_window = newwin(LINES - 3, COLS, 0, 0);
        components::draw_border(files_window, "LiveFind");

        files_derwin = components::create_derwin(files_window, 2, 2, 1, 1);

        files_menu = components::create_menu(files_window, files_derwin,
                                             files_items->data());

        wnoutrefresh(files_window);

        /////////////////////////
        // Create input window //
        /////////////////////////

        input_window = newwin(3, COLS, LINES - 3, 0);
        components::draw_border(input_window);

        input_derwin = components::create_derwin(input_window, 2, 2, 1, 1);

        input_fields[0] = new_field(1, COLS-2, 0, 0, 0, 0);
        input_fields[1] = nullptr;

        input_form = components::create_form(input_window, input_derwin, 
                                             input_fields);

        wnoutrefresh(input_window);

        move(LINES - 2, 1);
        doupdate();

        while (true) {
            static constexpr int CTRLC = 3;
            static constexpr int CTRLQ = 17;

            int input = getch();

            if (input == '\n' || input == KEY_ENTER) {
                std::string out = item_name(current_item(files_menu));
                utils::trim_whitespace(out);

                cleanup();
                endwin();

                return out;
            } else if (input == CTRLC || input == CTRLQ) {
                cleanup();
                endwin();

                return "";
            } else {
                process_input(input);
            }
        }
    }

    void cleanup()
    {
        // Directories window
        for (auto &item : *files_items) {
            free_item(item);
        }

        components::destroy_menu(files_menu);

        delete files_items;

        delwin(files_derwin);
        components::destroy_window(files_window);

        // Input window
        for (auto &field : input_fields) {
            free_field(field);
        }

        components::destroy_form(input_form);

        delwin(input_derwin);
        components::destroy_window(input_window);
    }

    void remove_files_items(const std::string &new_search)
    {
        // If the last edit to the search buffer was adding a char
        // we can just iterate through the existing directories

        components::destroy_menu(files_menu);

        int i = 0;
        while (i < files_items->size()) {
            ITEM *item = files_items->at(i);

            if (item == nullptr) break;

            std::string name{item_name(item)};

            if (name.find(new_search) == std::string::npos) {
                free_item(item);
                files_items->erase(files_items->begin() + i);
            } else {
                i++;
            }
        }

        werase(files_window);

        files_menu = components::create_menu(files_window, files_derwin,
                                             files_items->data());

        wnoutrefresh(files_derwin);
        wnoutrefresh(input_derwin);
        doupdate();
    }

    void add_files_items(const std::string &new_search)
    {
        components::destroy_menu(files_menu);

        files_items->pop_back(); // remove last nullptr

        auto prev_matches 
                = std::make_unique<std::unordered_set<std::string>>();

        for (const auto &item : *files_items) {
            prev_matches->insert(std::string{item_name(item)});
        }

        for (const auto &path : *paths) {
            if (prev_matches->find(path) == prev_matches->end()
                && path.find(new_search) != std::string::npos) {
                files_items->push_back(new_item(path.c_str(), nullptr));
            }
        }

        files_items->push_back(nullptr);

        werase(files_window);

        files_menu = components::create_menu(files_window, files_derwin,
                                             files_items->data());

        wnoutrefresh(files_derwin);
        wnoutrefresh(input_derwin);
        doupdate();
    }

    void process_input(const int ch)
    {
        std::string input;

        constexpr int CTRLN = 14;
        constexpr int CTRLP = 16;

        switch (ch) {
            case CTRLN:
            case KEY_DOWN:
                menu_driver(files_menu, REQ_NEXT_MATCH);

                curs_set(0);
                wrefresh(files_derwin);
                break;

            case CTRLP:
            case KEY_UP:
                menu_driver(files_menu, REQ_PREV_MATCH);

                curs_set(0);
                wrefresh(files_derwin);
                break;

            case KEY_BACKSPACE:
                form_driver(input_form, REQ_PREV_CHAR);
                form_driver(input_form, REQ_DEL_CHAR);
                form_driver(input_form, REQ_VALIDATION);

                input = field_buffer(input_fields[0], 0);
                utils::trim_whitespace(input);
                add_files_items(input);

                curs_set(1);
                move(LINES - 2, input.length() + 1);
                break;
            default:
                form_driver(input_form, ch);
                form_driver(input_form, REQ_VALIDATION);

                input = field_buffer(input_fields[0], 0);
                utils::trim_whitespace(input);
                remove_files_items(input);

                curs_set(1);
                move(LINES - 2, input.length() + 1);
                break;
        }
    }
}

