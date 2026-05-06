#pragma once

#include <string>
#include <vector>

namespace tui {
    std::string run(std::vector<std::string> paths);
    /// Description:
    ///     Starts the TUI application, including initializing ITEM pointers 
    ///     from the given paths list, opening dev/tty, setting up ncurses 
    ///     screen, capturing input and rendering things.
    ///
    /// Returns:
    ///     A string that the user has selected

    void cleanup();
    /// Description:
    ///     Frees dynamically allocated resources (entries, entriesCache) and 
    ///     releases ncurses widgets (entriesMenu/Window, inputForm/Fields/
    ///     Window). Also closes /dev/tty.

    void initializeEntries(const std::vector<std::string> &paths);
    /// Description:
    ///     Allocates entriesCache with the same size as paths list and 
    ///     creates ITEM *s. Then, allocates entries array with size of 
    ///     entriesCacheSize + 1 and copies all ITEM *s in entriesCache 
    ///     to this array, adding a nullptr at the end.

    void updateEntries(const std::string &searchString);
    /// Description:
    ///     Iterates through each ITEM * in entriesCache, checking if it has 
    ///     searchString as a substring in its name. If that is the case, it 
    ///     adds this object's item member to entries array. When finished, 
    ///     creates a nullptr after the very last added element.

    bool tryOpenTty();
    /// Description:
    ///     Tries to open /dev/tty.
    ///
    /// Returns:
    ///     True on success, false on failure

    void createEntriesComponents();
    /// Description:
    ///     Helper function that creates the main window with all of the 
    ///     entries (the window that is titled "livefind")

    void createInputComponents();
    /// Description:
    ///     Helper function that creates the input box

    void processInput(const int ch);
    /// Description:
    ///     Processes input:
    ///         C-n, C-j, Down  - Select next entry
    ///         C-p, C-k, Up    - Select previous entry
    ///         Backspace       - Erase letter
    ///         Other keys      - Append letter to input 
    ///                           and update matches
    ///
    ///     Note that this function does not handle Enter, C-c and C-q; 
    ///     these keys are handled by tui::run()
}

