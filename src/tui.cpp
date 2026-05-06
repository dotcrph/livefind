#include "tui.hpp"

#include <cstddef>
#include <cstdio>
#include <cstring>

#include <vector>
#include <string>

#include <ncurses.h>
#include <form.h>
#include <menu.h>

#include "utils.hpp"
#include "tui_components.hpp"

namespace tui {
    FILE *ttyFile;
    SCREEN *tty;

    WINDOW *entriesWindow,
           *entriesDerwin,
           *inputWindow,
           *inputDerwin;

    MENU *entriesMenu;

    FORM *inputForm;
    FIELD *inputFields[2];

    ITEM **entriesCache; 
    /// Description:
    ///     A vector with ITEM *s created in initializeEntries(). 
    ///     This should not be reassigned after initialization, as 
    ///     it is assumed by the program that the size of entries 
    ///     array is equal to entriesCacheSize + 1!

    size_t entriesCacheSize;
    /// Description:
    ///     Number of elements in entriesCache

    ITEM **entries; 
    /// Description:
    ///     Arena for all qualifying search entries terminated by nullptr.
    ///     Has size of entriesCache.size() + 1 (all entries + sentinel 
    ///     nullptr required by ncurses library). Regenerated in 
    ///     updateEntries().

    std::string run(std::vector<std::string> paths)
    {
        if (paths.empty()) {
            log::warning("No paths provided!");
            return "";
        }

        if(!tryOpenTty()) {
            return "";
        }

        initializeEntries(paths);

        noecho();
        cbreak();
        raw();

        // Disable \r => \n translation. This is 
        // to disable default Ctrl-j behaviour.
        nonl(); 
        keypad(stdscr, TRUE); // For arrow keys

        curs_set(1);

        refresh(); // A refresh is required to draw windows

        createEntriesComponents();
        createInputComponents();

        move(LINES - 2, 1);
        doupdate();

        while (true) {
            static constexpr int ENTER = 13;

            int input = getch();

            // mvprintw(1, COLS-10, "%s", std::to_string(input).c_str());

            // TODO: Move all of the things below in processInput()

            if (input == ENTER) {
                std::string selectedDir{};
                ITEM *currentDir = current_item(entriesMenu);

                if (currentDir != nullptr) {
                    selectedDir = item_name(currentDir);
                    utils::strTrimWhitespace(selectedDir);
                }

                cleanup();
                return selectedDir;
            } 

            if (input == conversions::ctrl('c') || 
                input == conversions::ctrl('q')) {
                cleanup();
                return "";
            } 

            processInput(input);
        }
    }

    void cleanup()
    {
        // Entries menu
        components::destroyMenu(entriesMenu);

        delwin(entriesDerwin);
        components::destroyWindow(entriesWindow);

        delete[] entries;

        for (size_t i = 0; i < entriesCacheSize; i++)
            free_item(entriesCache[i]);

        delete[] entriesCache;

        // Input form
        components::destroyForm(inputForm);

        for (FIELD *field : inputFields)
            free_field(field);

        delwin(inputDerwin);
        components::destroyWindow(inputWindow);

        // TUI
        endwin();

        delscreen(tty);
        fclose(ttyFile);
    }

    void initializeEntries(const std::vector<std::string> &paths)
    {
        log::verbose("Total directories scanned: %d", paths.size());

        entriesCacheSize = paths.size();
        entriesCache = new ITEM * [entriesCacheSize];

        for (size_t i = 0; i < entriesCacheSize; i++)
            entriesCache[i] = new_item(paths[i].c_str(), nullptr);

        // +1 for a sentinel nullptr required by ncurses
        entries = new ITEM * [entriesCacheSize + 1];

        for (size_t i = 0; i < entriesCacheSize; i++)
            entries[i] = entriesCache[i];

        entries[entriesCacheSize] = nullptr;
    }

    void updateEntries(const std::string &searchString)
    {
        components::destroyMenu(entriesMenu);
        werase(entriesWindow);

        ITEM **nextFree = entries;
        for (size_t i = 0; i < entriesCacheSize; i++) {
            ITEM *item = entriesCache[i];
            const char *name = item_name(item);

            if (!strstr(name, searchString.c_str()))
                continue;

            *nextFree = item;
            nextFree++;
        }

        *nextFree = nullptr; // Sentinel required by ncurses

        entriesMenu = components::createMenu(
            entriesWindow, 
            entriesDerwin, 
            entries
        );

        wnoutrefresh(entriesDerwin);
        wnoutrefresh(inputDerwin);
        doupdate();
    }

    bool tryOpenTty()
    {
        ttyFile = fopen("/dev/tty", "r+");
        if (!ttyFile) {
            log::error("Failed to open /dev/tty!");
            return false;
        }

        tty = newterm(nullptr, ttyFile, ttyFile); // nullptr = $TERM here
        if (!tty) {
            log::error("Failed to create a new terminal from /dev/tty!");
            fclose(ttyFile);
            return false;
        }

        set_term(tty);
        return true;
    }

    void createEntriesComponents()
    {
        entriesWindow = newwin(LINES - 3, COLS, 0, 0);
        components::drawBorder(entriesWindow, "livefind");

        entriesDerwin = components::createDerwin(entriesWindow, 1, 1, 1, 1);

        entriesMenu = components::createMenu(
            entriesWindow, 
            entriesDerwin,
            entries
        );

        wnoutrefresh(entriesWindow);
    }

    void createInputComponents()
    {
        inputWindow = newwin(3, COLS, LINES - 3, 0);
        components::drawBorder(inputWindow);

        inputDerwin = components::createDerwin(inputWindow, 1, 1, 1, 1);

        inputFields[0] = new_field(1, COLS-2, 0, 0, 0, 0);
        inputFields[1] = nullptr;

        inputForm = components::createForm(
            inputWindow, 
            inputDerwin, 
            inputFields
        );

        wnoutrefresh(inputWindow);
    }

    void processInput(const int ch)
    {
        // TODO: Add horizontal cursor movement/editing
        std::string input;

        switch (ch) {
            case conversions::ctrl('n'):
            case conversions::ctrl('j'):
            case KEY_DOWN:
                menu_driver(entriesMenu, REQ_NEXT_MATCH);

                curs_set(0);
                wrefresh(entriesDerwin);
                break;

            case conversions::ctrl('p'):
            case conversions::ctrl('k'):
            case KEY_UP:
                menu_driver(entriesMenu, REQ_PREV_MATCH);

                curs_set(0);
                wrefresh(entriesDerwin);
                break;

            case KEY_BACKSPACE:
                form_driver(inputForm, REQ_PREV_CHAR);
                form_driver(inputForm, REQ_DEL_CHAR);
                form_driver(inputForm, REQ_VALIDATION);

                input = field_buffer(inputFields[0], 0);
                utils::strTrimWhitespace(input);
                updateEntries(input);

                curs_set(1);
                move(LINES - 2, input.length() + 1);
                break;
            default:
                form_driver(inputForm, ch);
                form_driver(inputForm, REQ_VALIDATION);

                // FIXME: Check if a character is a display character.
                // When pressing tab, for example, updateEntries() still 
                // runs, and that resets user selection

                input = field_buffer(inputFields[0], 0);
                utils::strTrimWhitespace(input);
                updateEntries(input);

                curs_set(1);
                move(LINES - 2, input.length() + 1);
                break;
        }
    }
}

