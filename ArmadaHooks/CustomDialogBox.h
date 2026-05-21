// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#pragma once

#include <list>

#define RT_DIALOG_A     MAKEINTRESOURCEA(5)

// Define DLGTEMPLATEEX (not provided by Windows headers)
#pragma pack(push, 1)
typedef struct {
    WORD dlgVer;
    WORD signature;     // Must be 0xFFFF
    DWORD helpID;
    DWORD exStyle;
    DWORD style;
    WORD cDlgItems;
    short x;
    short y;
    short cx;
    short cy;
    // Followed by variable data (menu, class, title, font...)
} DLGTEMPLATEEX;
#pragma pack(pop)


struct DialogHandle
{
    HWND window;
    bool popup;
    bool closeFlag;
    INT_PTR result;
};


class CustomDialogBox
{
private:
    // STATIC

    static std::list<DialogHandle*> dialogHandles;

    // INSTANCE

    void Create();
    void Destroy();

    void RunMessageLoop();

    HINSTANCE hInstance;
    LPCSTR lpTemplateName;
    HWND hWndParent;
    DLGPROC lpDialogFunc;
    LPARAM dwInitParam;

    DialogHandle handle{};

public:
    // STATIC

    /* Reposition the dialog windows on parent window move. */
    /* The RECT should be in screen coordinates. */
    static void NotifyMove(const RECT& newPos);

    /* Tell a dialog to close. */
    /* This is equivalent to the EndDialog API */
    static bool NotifyClose(HWND hDlg, INT_PTR result);

    /* Returns true iff the handle belongs to the CustomDialogBox class. */
    static bool IsCustomDialog(HWND hDlg);

    // INSTANCE

    /* Initialize the dialog wrapper with the original API parameters. */
    CustomDialogBox(
        HINSTANCE hInstance,
        LPCSTR lpTemplateName,
        HWND hWndParent,
        DLGPROC lpDialogFunc,
        LPARAM dwInitParam
    );

    /* Simulate a blocking modal dialog. */
    /* If popup is true, the previous dialog will be locked instead of hidden. */
    INT_PTR Run(bool popup = false);
};

