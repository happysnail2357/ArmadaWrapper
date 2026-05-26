// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#include "pch.h"
#include "User32Wrap.h"
#include "CustomDialogBox.h"
#include "MouseManager.h"
#include "BinkOverlayWindow.h"
#include "TrueApi.h"

#include "Binkw32Wrap.h"


/************  Global Variables  ************/

// Flag to indicate main gameplay
bool inGame{ false };

// Menu flags
bool isEscapeMenu{ false };
bool isLoadGameMenu{ false };
bool abortMissionFlag{ false };

// Mouse cursor handling
MouseManager mouse{};
HCURSOR lastCursor{};

// Armada window 
HWND armadaWindow{ nullptr };
ATOM armadaClass{};
WNDPROC armadaWindowProcedure{ nullptr };

// Menu animation window
extern BinkOverlayWindow binkWindow{};
DLGPROC mainMenuProcedure{ nullptr };

// While debugging the disassembly I found that
// the dialog menus place values at this address
// depending on which dialog button is pressed.
const DWORD* dialogResultFlag = reinterpret_cast<DWORD*>(0x68B8C0);


/************** Non-API Functions **************/

// Helper to convert window coordinates to screen coordinates
bool ClientRectToScreenRect(HWND hwnd, PRECT box)
{
    POINT pt;
    pt.x = box->left;
    pt.y = box->top;

    if (!ClientToScreen(hwnd, &pt)) return false;

    box->left = pt.x;
    box->top = pt.y;

    pt.x = box->right;
    pt.y = box->bottom;

    if (!ClientToScreen(hwnd, &pt)) return false;

    box->right = pt.x;
    box->bottom = pt.y;

    return true;
}

void NotifyNewWindowPos(HWND hwnd)
{
    RECT clientBox;
    if (GetClientRect(hwnd, &clientBox))
    {
        RECT screenBox = clientBox;
        if (ClientRectToScreenRect(hwnd, &screenBox))
        {
            CustomDialogBox::NotifyMove(screenBox);
            binkWindow.NotifyMove(screenBox);
        }

        mouse.UpdateNeutralPosition(hwnd, clientBox);
    }
}

// Window procedure to intercept messages sent to the Armada window
LRESULT CALLBACK RelayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_ACTIVATE:
        switch (wParam)
        {
        case WA_ACTIVE:
            mouse.UpdateNeutralPosition(hwnd);
            binkWindow.Jump();
            break;

        case WA_CLICKACTIVE:
            if (inGame)
            {
                mouse.UpdateNeutralPosition(hwnd);
                mouse.Capture();

                // Prevent a drag inside the game by releasing
                // the mouse click immediately.
                mouse.Unclick(hwnd);
            }
            binkWindow.Jump();
            break;

        case WA_INACTIVE:
            if (inGame)
            {
                mouse.Release();
            }
            break;
        }
        break;

    case WM_SIZE:
        if (wParam == SIZE_MAXIMIZED)
        {
            mouse.UpdateNeutralPosition(hwnd);
        }
        break;

    case WM_WINDOWPOSCHANGED:
        {
            NotifyNewWindowPos(hwnd);
        }
        break;
    case WM_SYSKEYDOWN:
        {
            if (wParam == VK_RETURN)
            {
                return DefWindowProc(hwnd, msg, wParam, lParam);
            }
            else if (wParam == VK_MENU)
            {
                if (inGame)
                {
                    if (!mouse.IsCaptured())
                    {
                        mouse.Capture();
                    }
                    else
                    {
                        mouse.Release();
                    }
                }
            }
        }
        break;
    }

    // Call the real handler
    LRESULT result = CallWindowProc(armadaWindowProcedure, hwnd, msg, wParam, lParam);

    if (msg == WM_CLOSE)
    {
        DEBUG_PRINT(TEXT("Window is closing !!!"));

        if (!inGame)
        {
            // Well... this is one way to force the game to close :)
            ExitProcess(1701);
        }
    }
    else if (msg == WM_DESTROY)
    {
        DEBUG_PRINT(TEXT("Window is being destroyed !!!"));
    }

    return result;
}


/*******************  API Hooks  *******************/

ATOM WINAPI WrapRegisterClassA(WNDCLASSA* lpWndClass)
{
    if (lpWndClass)
    {
        if (reinterpret_cast<uintptr_t>(lpWndClass->lpszClassName) > 0xFFFF)
        {
            if (strcmp(lpWndClass->lpszClassName, "Armada") == 0)
            {
                // Save the true Armada window procedure and insert our own procedure
                armadaWindowProcedure = lpWndClass->lpfnWndProc;
                lpWndClass->lpfnWndProc = RelayWndProc;
                armadaClass = TrueApi::api.user32.RegisterClassA(lpWndClass);
                return armadaClass;
            }
        }
    }

    return TrueApi::api.user32.RegisterClassA(lpWndClass);
}

HWND WINAPI WrapCreateWindowExA(
    DWORD     dwExStyle,
    LPCSTR    lpClassName,
    LPCSTR    lpWindowName,
    DWORD     dwStyle,
    int       X,
    int       Y,
    int       nWidth,
    int       nHeight,
    HWND      hWndParent,
    HMENU     hMenu,
    HINSTANCE hInstance,
    LPVOID    lpParam
)
{
    bool isArmadaWindow = false;
    
    uintptr_t classNameAsInt = reinterpret_cast<uintptr_t>(lpClassName);

    if (classNameAsInt > 0xFFFF)
    {
        if (strcmp(lpClassName, "Armada") == 0)
        {
            isArmadaWindow = true;
        }
    }
    else if (static_cast<ATOM>(classNameAsInt) == armadaClass)
    {
        isArmadaWindow = true;
    }

    HWND windowHandle = TrueApi::api.user32.CreateWindowExA(
        dwExStyle,
        lpClassName,
        lpWindowName,
        dwStyle,
        X,
        Y,
        nWidth,
        nHeight,
        hWndParent,
        hMenu,
        hInstance,
        lpParam
    );

    if (isArmadaWindow)
    {
        armadaWindow = windowHandle;
        binkWindow.Create(armadaWindow);
    }

    return windowHandle;
}

BOOL WINAPI WrapDestroyWindow(HWND hWnd)
{
    if (hWnd == armadaWindow)
    {
        binkWindow.Destroy();
    }

    return TrueApi::api.user32.DestroyWindow(hWnd);
}


BOOL WINAPI WrapGetCursorPos(LPPOINT lpPoint)
{
    if (mouse.IsCaptured())
    {
        return TrueApi::api.user32.GetCursorPos(lpPoint);
    }

    // If the mouse is not captured by the game,
    // tell the game that the mouse is in the middle
    // of the screen (which it interprets as no
    // game-mouse movement)

    mouse.RetrieveNeutralPosition(lpPoint);

    return TRUE;
}

BOOL WINAPI WrapSetCursorPos(int X, int Y)
{
    if (mouse.IsCaptured())
    {
        return TrueApi::api.user32.SetCursorPos(X, Y);
    }

    // If the mouse is not captured by the game,
    // tell the game that the mouse was moved,
    // but don't actually move it.

    return TRUE;
}

HCURSOR WINAPI WrapSetCursor(HCURSOR hCursor)
{
    if (mouse.IsCaptured())
    {
        lastCursor = TrueApi::api.user32.SetCursor(hCursor);
    }

    // If the mouse is not captured by the game,
    // don't let the game change the mouse cursor.
    // It may try setting the cursor to CURSOR_INVISIBLE.
    
    return lastCursor;
}


INT_PTR WINAPI WrapDialogBoxParamA(
    HINSTANCE hInstance,
    LPCSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam
)
{
    ATOM dialogId{};
    uintptr_t templateNameAsInt = reinterpret_cast<uintptr_t>(lpTemplateName);

    if (templateNameAsInt <= 0xFFFF)
    {
        dialogId = static_cast<ATOM>(templateNameAsInt);
    }

    if (inGame)
    {
        // If a dialog box is being created,
        // then the game is either paused or ended

        inGame = false;
        mouse.Share();

        if (dialogId == 291)
        {
            isEscapeMenu = true;
        }
    }

    if (mainMenuProcedure == nullptr && dialogId == 0x123)
    {
        mainMenuProcedure = lpDialogFunc;

        NotifyNewWindowPos(armadaWindow);
    }

    bool containsAnimations = (dialogId == 0x123 && lpDialogFunc == mainMenuProcedure) ||
        (dialogId == 0x073 && reinterpret_cast<int>(lpDialogFunc) == 0x005499c0);

    if (containsAnimations)
    {
        binkWindow.Show();
    }
    else if (dialogId == 0x124)
    {
        binkWindow.Hide();
    }

    // The actual DialogBoxParamA API is not used
    // here because it disables the parent window.
    // Instead, a custom dialog class is used which
    // utitlizes CreateDialogIndirectParamA.

    CustomDialogBox dialog(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);

    bool popup = dialogId == 0x124 || dialogId == 0x87f || dialogId == 0x873 || (dialogId == 0x73 && isEscapeMenu);

    INT_PTR result = dialog.Run(popup);

    if (containsAnimations)
    {
        binkWindow.Hide();
    }
    else if (dialogId == 0x124 && result == 0)
    {
        binkWindow.Show();
    }

    // The dialog number and result value are used to determine
    // which screen the game is on. We need to do this so we 
    // know when the game transitions to the "in game" RTS screen.

    if (result == 1)
    {
        // Multiplayer setup and single player select dialogs
        if (dialogId == 2096 || dialogId == 292)
        {
            inGame = true;
            hoveredTextTarget = BinkMovie::None;
        }
        // Generic menu dialog
        else if (dialogId == 291)
        {
            if (isEscapeMenu)
            {
                isEscapeMenu = false;

                // "Return to Game", "Restate Objectives", or "Abort Mission"
                if (*dialogResultFlag == 0x0)
                {
                    if (!abortMissionFlag)
                    {
                        //DEBUG_PRINT(TEXT("Entering Game"));
                        inGame = true;
                    }
                    else
                    {
                        // Clear the flag for next time
                        abortMissionFlag = false;
                        binkWindow.Show();
                    }
                }
                // "Load Game"
                else if (*dialogResultFlag == 0x18)
                {
                    //DEBUG_PRINT(TEXT("Entering Load Game menu"));
                    isLoadGameMenu = true;
                }
            }
            else if (isLoadGameMenu)
            {
                isLoadGameMenu = false;

                // User chose to load a game
                if (*dialogResultFlag == 0x0)
                {
                    //DEBUG_PRINT(TEXT("Entering Game"));
                    inGame = true;
                }
                // User chose to return to previous menu (escape menu)
                else if (*dialogResultFlag == 0xB)
                {
                    //DEBUG_PRINT(TEXT("Exiting Load Game menu"));
                    isEscapeMenu = true;
                }
                // User chose to return to previous menu (single player menu)
                // else if (*dialogResultFlag == 0x2)
            }
        }
        // Messagebox dialog
        else if (dialogId == 115)
        {
            // "Abort Mission" popup
            if (isEscapeMenu)
            {
                abortMissionFlag = true;
            }
            // "Single Player" menu to "Load Game" menu
            else if (*dialogResultFlag == 0x12)
            {
                isLoadGameMenu = true;
            }
        }
    }

    return result;
}

BOOL WINAPI WrapEndDialog(HWND hDlg, INT_PTR nResult)
{
    // Since we have a fancy custom dialog class
    // that manages its own window, we need to have
    // it close the dialog for us.

    if (CustomDialogBox::NotifyClose(hDlg, nResult))
    {
        return TRUE;
    }

    return TrueApi::api.user32.EndDialog(hDlg, nResult);
}


LONG WINAPI WrapSetWindowLongA(HWND hwnd, int nIndex, LONG dwNewLong)
{
    if (hwnd == armadaWindow)
    {
        if (nIndex == GWL_WNDPROC)
        {
            // The game likes to switch out the window procedure.
            // We'll have to simulate this because we're
            // using our own window procedure.
            LONG oldProc = reinterpret_cast<LONG>(armadaWindowProcedure);
            armadaWindowProcedure = reinterpret_cast<WNDPROC>(dwNewLong);
            return oldProc;
        }
    }

    return TrueApi::api.user32.SetWindowLongA(hwnd, nIndex, dwNewLong);
}

LONG WINAPI WrapGetWindowLongA(HWND hwnd, int nIndex)
{
    if (hwnd == armadaWindow)
    {
        if (nIndex == GWL_WNDPROC)
        {
            // Return the pointer to the real armada window procedure
            // like the game expects. We have to do this because
            // the window is actually using our window procedure.
            return reinterpret_cast<LONG>(armadaWindowProcedure);
        }
    }

    return TrueApi::api.user32.GetWindowLongA(hwnd, nIndex);
}


int WINAPI WrapDrawTextA(
    HDC hdc,
    LPCSTR lpchText,
    int cchText,
    LPRECT lprc,
    UINT format)
{
    RECT offsetRect;

    // Check if the bink hooks need text drawn over an animation 
    switch (hoveredTextTarget)
    {
    case BinkMovie::KlingonHover:
        offsetRect.left = offsetRect.right = 360;
        offsetRect.top = offsetRect.bottom = 30;
        break;

    case BinkMovie::RomulanHover:
        offsetRect.left = offsetRect.right = 0;
        offsetRect.top = offsetRect.bottom = 250;
        break;

    case BinkMovie::BorgHover:
        offsetRect.left = offsetRect.right = 360;
        offsetRect.top = offsetRect.bottom = 250;
        break;

    default:
        return TrueApi::api.user32.DrawTextA(hdc, lpchText, cchText, lprc, format);
    }

    // We'll redirect this text to the overlay window
    HDC buffer = binkWindow.GetBufferDC();

    if (buffer)
    {
        offsetRect.right += lprc->right;
        offsetRect.bottom += lprc->bottom;

        return TrueApi::api.user32.DrawTextA(buffer, lpchText, cchText, &offsetRect, format);
    }
    else
    {
        return TrueApi::api.user32.DrawTextA(hdc, lpchText, cchText, lprc, format);
    }
}
