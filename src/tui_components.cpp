#include "tui_components.hpp"

#include <cctype>
#include <cstdio>
#include <cstdarg>

#include <string>

#include <ncurses.h>
#include <menu.h>
#include <form.h>

namespace tui::components {
    void drawBorder(WINDOW *windowPtr, const std::string &title)
    {
        static constexpr int TITLE_X_OFFSET = 2;

        box(windowPtr, 0, 0);

        if (title.empty())
            return;

        wattron(windowPtr, A_REVERSE);

        mvwaddch(windowPtr, 0, TITLE_X_OFFSET, ' ');
        mvwprintw(windowPtr, 0, TITLE_X_OFFSET + 1, "%s", title.c_str());
        mvwaddch(windowPtr, 0, TITLE_X_OFFSET + title.length() + 1, ' ');

        wattroff(windowPtr, A_REVERSE);
    }

    WINDOW *createDerwin(
        WINDOW *parentWin, 
        int marginTop, int marginLeft, 
        int marginBottom, int marginRight)
    {
        int parentH, parentW;
        getmaxyx(parentWin, parentH, parentW);

        return derwin(
            parentWin, 
            parentH - marginBottom - marginTop, 
            parentW - marginRight - marginLeft, 
            marginTop, 
            marginLeft
        );
    }

    void destroyWindow(WINDOW *windowPtr)
    {
        wborder(windowPtr, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        wrefresh(windowPtr);
        delwin(windowPtr);
    }

    MENU *createMenu(WINDOW *parentWin, WINDOW *subWin, ITEM **items)
    {
        MENU *menuPtr = new_menu(items);

        int parentH, parentW;
        getmaxyx(parentWin, parentH, parentW);

        set_menu_win(menuPtr, parentWin);
        set_menu_sub(menuPtr, subWin);
        set_menu_format(menuPtr, parentH - 2, 1);
        set_menu_mark(menuPtr, " > ");

        post_menu(menuPtr);
        return menuPtr;
    }

    void destroyMenu(MENU *menuPtr)
    {
        unpost_menu(menuPtr);
        free_menu(menuPtr);
    }

    FORM *createForm(WINDOW *parentWin, WINDOW *subWin, FIELD **fields)
    {
        FORM *formPtr = new_form(fields);

        set_form_win(formPtr, parentWin);
        set_form_sub(formPtr, subWin);

        post_form(formPtr);
        return formPtr;
    }

    void destroyForm(FORM *formPtr)
    {
        unpost_form(formPtr);
        free_form(formPtr);
    }
}
