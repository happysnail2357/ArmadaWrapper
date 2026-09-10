// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#include "pch.h"
#include "User32Wrap.h"
#include "HookAssets.h"

#include "CustomDialogBox.h"


/************  Global Variables  ************/

// Mouse cursor management
HCURSOR lastCursor{};

// Armada window management
HWND armadaWindow{ nullptr };
ATOM armadaClass{};
WNDPROC armadaWindowProcedure{ nullptr };


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
            HookAssets::binkWindow.NotifyMove(screenBox);
        }

        HookAssets::mouse.UpdateNeutralPosition(hwnd, clientBox);
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
            HookAssets::mouse.UpdateNeutralPosition(hwnd);
            HookAssets::binkWindow.Jump();

            if (!HookAssets::state.IsInGame() && HookAssets::state.UsingMapEditor())
            {
                if (CustomDialogBox::ActivateTopmost())
                {
                    return DefWindowProc(hwnd, msg, wParam, lParam);
                }
            }
            break;

        case WA_CLICKACTIVE:
            if (HookAssets::state.IsInGame())
            {
                HookAssets::mouse.UpdateNeutralPosition(hwnd);
                HookAssets::mouse.Capture();

                // Prevent a drag inside the game by releasing
                // the mouse click immediately.
                HookAssets::mouse.Unclick(hwnd);
            }
            else if (HookAssets::state.UsingMapEditor())
            {
                return DefWindowProc(hwnd, msg, wParam, lParam);
            }
            HookAssets::binkWindow.Jump();
            break;

        case WA_INACTIVE:
            if (HookAssets::state.IsInGame())
            {
                HookAssets::mouse.Release();
            }
            break;
        }
        break;

    case WM_SIZE:
        if (wParam == SIZE_MAXIMIZED)
        {
            HookAssets::mouse.UpdateNeutralPosition(hwnd);
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
                if (HookAssets::state.IsInGame())
                {
                    if (!HookAssets::mouse.IsCaptured())
                    {
                        HookAssets::mouse.Capture();
                    }
                    else
                    {
                        HookAssets::mouse.Release();
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
        // It appears the game developers never meant for the game to be closed
        // via the "X" button in the window title bar. The game will happily run
        // as a background process after the window closes. So, we need to force
        // kill the game when its window closes.

        DEBUG_PRINT(TEXT("Forcing process exit"));

        // Well... this is one way to force the game to close :)
        ExitProcess(1701);
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
        HookAssets::binkWindow.Create(armadaWindow);
    }

    return windowHandle;
}

BOOL WINAPI WrapDestroyWindow(HWND hWnd)
{
    if (hWnd == armadaWindow)
    {
        HookAssets::binkWindow.Destroy();
    }

    return TrueApi::api.user32.DestroyWindow(hWnd);
}


BOOL WINAPI WrapGetCursorPos(LPPOINT lpPoint)
{
    if (HookAssets::mouse.IsCaptured())
    {
        return TrueApi::api.user32.GetCursorPos(lpPoint);
    }

    // If the mouse is not captured by the game,
    // tell the game that the mouse is in the middle
    // of the screen (which it interprets as no
    // game-mouse movement)

    HookAssets::mouse.RetrieveNeutralPosition(lpPoint);

    return TRUE;
}

BOOL WINAPI WrapSetCursorPos(int X, int Y)
{
    if (HookAssets::mouse.IsCaptured())
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
    if (HookAssets::mouse.IsCaptured())
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
    // The actual DialogBoxParamA API is not used
    // here because it disables the parent window.
    // Instead, a custom dialog class is used which
    // utitlizes CreateDialogIndirectParamA.

    CustomDialogBox dialog(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);

    if (HookAssets::state.IsInGame())
    {
        HookAssets::mouse.Share();
    }

    DialogContext dialogContext = HookAssets::state.ReportDialogCreation(dialog.Id(), lpDialogFunc);

    bool isAnimatedMenu = dialogContext == DialogContext::MainMenu ||
                          dialogContext == DialogContext::SinglePlayerMenu;

    if (isAnimatedMenu)
    {
        NotifyNewWindowPos(armadaWindow);
        HookAssets::binkWindow.Show();
    }
    else if (dialogContext == DialogContext::MissionSelectPopup)
    {
        HookAssets::binkWindow.Freeze();
    }

    bool popup = StateManager::DialogIsPopup(dialogContext);

    INT_PTR result = dialog.Run(popup);

    if (isAnimatedMenu)
    {
        HookAssets::binkWindow.Hide();
    }
    else if (result == 0 && dialogContext == DialogContext::MissionSelectPopup)
    {
        HookAssets::binkWindow.Show();
    }
    else if (result == 1 && dialogContext == DialogContext::AbortMissionPopup)
    {
        HookAssets::binkWindow.Show();
    }

    HookAssets::state.ReportDialogClose(dialogContext, result);

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

    // Check if the bink wrapper needs text drawn over an animation
    switch (HookAssets::state.textRenderTarget)
    {
    case MenuAnimation::Klingon:
        offsetRect.left = offsetRect.right = 360;
        offsetRect.top = offsetRect.bottom = 30;
        break;

    case MenuAnimation::Romulan:
        offsetRect.left = offsetRect.right = 0;
        offsetRect.top = offsetRect.bottom = 250;
        break;

    case MenuAnimation::Borg:
        offsetRect.left = offsetRect.right = 360;
        offsetRect.top = offsetRect.bottom = 250;
        break;

    default:
        return TrueApi::api.user32.DrawTextA(hdc, lpchText, cchText, lprc, format);
    }

    // We'll redirect this text to the overlay window
    HDC buffer = HookAssets::binkWindow.GetBufferDC();

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
