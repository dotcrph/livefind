#pragma once

#include <string>

#include <ncurses.h>
#include <menu.h>
#include <form.h>

namespace tui::components {
    void drawBorder(WINDOW *windowPtr, const std::string &title = "");
    /// Description:
    ///     A wrapper around ncurses' box(). Draws a border around a window 
    ///     and optionally renders a title with reverse video at the top.

    WINDOW *createDerwin(
        WINDOW *parentWin, 
        int marginBottom, int marginRight, 
        int marginTop, int marginLeft);
    /// Description:
    ///     A wrapper around ncurses' derwin(). Creates a derived window with 
    ///     parentWin as parent and sets the derived window's dimensions to 
    ///     be the same as it's parent's minus the margin.
    ///
    /// Returns:
    ///     Pointer to the derived window

    void destroyWindow(WINDOW *windowPtr);
    /// Description:
    ///     A wrapper around ncurses' delwin(). Erases the border an 
    ///     refreshes the screen before freeing the window.

    MENU *createMenu(WINDOW *parentWin, WINDOW *subWin, ITEM **items);
    /// Description:
    ///     Creates a new menu from an array of pointers to ITEMs, sets 
    ///     parentWin as a parent and subWin as a subwindow, sets menu 
    ///     dimensions to ones of parentWin and calls ncurses' post_menu().
    ///
    /// Returns:
    ///     Pointer to the created menu

    void destroyMenu(MENU *menuPtr);
    /// Description:
    ///     Calls ncurses' unpost_menu() and free_menu().

    FORM *createForm(WINDOW *parentWin, WINDOW *subWin, FIELD **fields);
    /// Description:
    ///     Creates a new form from an array of pointers to FIELDs, sets 
    ///     parentWin as a parent and subWin as a subwindow and calls ncurses' 
    ///     post_form().
    ///
    /// Returns:
    ///     Pointer to the created form

    void destroyForm(FORM *formPtr);
    /// Description:
    ///     Calls ncurses' unpost_form() and free_form().
}

