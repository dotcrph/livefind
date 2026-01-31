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
    FILE *tty_file;
    SCREEN *tty;

    WINDOW *dirs_window;
    WINDOW *dirs_derwin;
    WINDOW *input_window;
    WINDOW *input_derwin;

    MENU *dirs_menu;
    std::vector<ITEM*> *dirs_entries;

    FORM *input_form;
    FIELD *input_fields[2];

    void cleanup();
    bool try_open_tty();

    void create_dirs_components();
    void create_input_components();

    void remove_dirs_entries(const std::string &new_search);
    void add_dirs_entries(const std::string &new_search);
    void process_input(const int ch);

    std::string run()
    {
        dirs_entries = new std::vector<ITEM*>();

        for (const auto &path : *paths) {
            dirs_entries->push_back(new_item(path.c_str(), nullptr));
        }
        dirs_entries->push_back(nullptr);

        if(!try_open_tty())
        {
            cleanup();
            return "";
        }

        noecho();
        cbreak();
        raw();
        nonl(); // Disable \r => \n translation
                // WARNING: probably will break stuff on windows 
                // (if i'll ever decide to port this)
        keypad(stdscr, TRUE); // For arrow keys

        curs_set(1);

        refresh(); // A refresh is required to draw windows

        create_dirs_components();
        create_input_components();

        move(LINES - 2, 1);
        doupdate();

        while (true) {
            static constexpr int ENTER = 13;

            int input = getch();

            // mvprintw(1, COLS-10, "%s", std::to_string(input).c_str());

            if (input == ENTER) {
                std::string out{};
                ITEM *cur_dir = current_item(dirs_menu);

                if (cur_dir != nullptr) {
                    out = item_name(cur_dir);
                    utils::trim_whitespace(out);
                }

                cleanup();

                return out;
            } else if (input == conversions::ctrl('c') 
                       || input == conversions::ctrl('q')) {
                cleanup();

                return "";
            } else {
                process_input(input);
            }
        }
    }

    void cleanup()
    {
        // Directories window
        for (auto &item : *dirs_entries) {
            free_item(item);
        }

        components::destroy_menu(dirs_menu);

        delete dirs_entries;

        delwin(dirs_derwin);
        components::destroy_window(dirs_window);

        // Input window
        for (auto &field : input_fields) {
            free_field(field);
        }

        components::destroy_form(input_form);

        delwin(input_derwin);
        components::destroy_window(input_window);

        endwin();

        delscreen(tty);
        fclose(tty_file);
    }

    bool try_open_tty()
    {
        tty_file = fopen("/dev/tty", "r+");
        if (!tty_file)
        {
            log::error("Failed to open /dev/tty!");
            return false;
        }

        tty = newterm(nullptr, tty_file, tty_file); // nullptr = $TERM here
        if (!tty_file)
        {
            log::error("Failed to create a new terminal from /dev/tty!");
            return false;
        }

        set_term(tty);
        return true;
    }

    void create_dirs_components()
    {
        dirs_window = newwin(LINES - 3, COLS, 0, 0);
        components::draw_border(dirs_window, "LiveFind");

        dirs_derwin = components::create_derwin(dirs_window, 2, 2, 1, 1);

        dirs_menu = components::create_menu(dirs_window, dirs_derwin,
                                             dirs_entries->data());

        wnoutrefresh(dirs_window);

    }

    void create_input_components()
    {
        input_window = newwin(3, COLS, LINES - 3, 0);
        components::draw_border(input_window);

        input_derwin = components::create_derwin(input_window, 2, 2, 1, 1);

        input_fields[0] = new_field(1, COLS-2, 0, 0, 0, 0);
        input_fields[1] = nullptr;

        input_form = components::create_form(input_window, input_derwin, 
                                             input_fields);

        wnoutrefresh(input_window);
    }

    void remove_dirs_entries(const std::string &new_search)
    {
        // If the last edit to the search buffer was adding a char
        // we can just iterate through the existing directories

        components::destroy_menu(dirs_menu);

        int i = 0;
        while (i < dirs_entries->size()) {
            ITEM *item = dirs_entries->at(i);

            if (item == nullptr) break;

            std::string name{item_name(item)};

            if (name.find(new_search) == std::string::npos) {
                free_item(item);
                dirs_entries->erase(dirs_entries->begin() + i);
            } else {
                i++;
            }
        }

        werase(dirs_window);

        dirs_menu = components::create_menu(dirs_window, dirs_derwin,
                                             dirs_entries->data());

        wnoutrefresh(dirs_derwin);
        wnoutrefresh(input_derwin);
        doupdate();
    }

    void add_dirs_entries(const std::string &new_search)
    {
        components::destroy_menu(dirs_menu);

        dirs_entries->pop_back(); // remove last nullptr

        auto prev_matches 
                = std::make_unique<std::unordered_set<std::string>>();

        for (const auto &item : *dirs_entries) {
            prev_matches->insert(std::string{item_name(item)});
        }

        for (const auto &path : *paths) {
            if (prev_matches->find(path) == prev_matches->end()
                && path.find(new_search) != std::string::npos) {
                dirs_entries->push_back(new_item(path.c_str(), nullptr));
            }
        }

        dirs_entries->push_back(nullptr);

        werase(dirs_window);

        dirs_menu = components::create_menu(dirs_window, dirs_derwin,
                                             dirs_entries->data());

        wnoutrefresh(dirs_derwin);
        wnoutrefresh(input_derwin);
        doupdate();
    }

    void process_input(const int ch)
    {
        std::string input;

        switch (ch) {
            case conversions::ctrl('n'):
            case conversions::ctrl('j'):
            case KEY_DOWN:
                menu_driver(dirs_menu, REQ_NEXT_MATCH);

                curs_set(0);
                wrefresh(dirs_derwin);
                break;

            case conversions::ctrl('p'):
            case conversions::ctrl('k'):
            case KEY_UP:
                menu_driver(dirs_menu, REQ_PREV_MATCH);

                curs_set(0);
                wrefresh(dirs_derwin);
                break;

            case KEY_BACKSPACE:
                form_driver(input_form, REQ_PREV_CHAR);
                form_driver(input_form, REQ_DEL_CHAR);
                form_driver(input_form, REQ_VALIDATION);

                input = field_buffer(input_fields[0], 0);
                utils::trim_whitespace(input);
                add_dirs_entries(input);

                curs_set(1);
                move(LINES - 2, input.length() + 1);
                break;
            default:
                form_driver(input_form, ch);
                form_driver(input_form, REQ_VALIDATION);

                input = field_buffer(input_fields[0], 0);
                utils::trim_whitespace(input);
                remove_dirs_entries(input);

                curs_set(1);
                move(LINES - 2, input.length() + 1);
                break;
        }
    }
}

