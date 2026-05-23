// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#include "pch.h"
#include "MouseManager.h"
#include "TrueApi.h"


void MouseManager::ShowMouse()
{
    while (ShowCursor(TRUE) < 0);
}

void MouseManager::HideMouse()
{
    while (ShowCursor(FALSE) >= 0);
}

bool MouseManager::Capture()
{
    if (!this->captured)
    {
        HideMouse();
        this->SetNeutral();
        this->captured = true;
        return true;
    }

    return false;
}

void MouseManager::Share()
{
    if (!this->captured)
    {
        this->captured = true;
    }
}

void MouseManager::Release()
{
    if (this->captured)
    {
        this->captured = false;
        ShowMouse();
    }
}

void MouseManager::Unclick(HWND hwnd)
{
    PostMessage(hwnd, WM_LBUTTONUP, 0, MAKELPARAM(this->neutralPosition.x, this->neutralPosition.y));
}

bool MouseManager::IsCaptured()
{
    return this->captured;
}

void MouseManager::UpdateNeutralPosition(HWND hwnd)
{
    RECT clientArea;

    if (GetClientRect(hwnd, &clientArea))
    {
        UpdateNeutralPosition(hwnd, clientArea);
    }
}

void MouseManager::UpdateNeutralPosition(HWND hwnd, RECT clientArea)
{
    POINT center;

    center.x = clientArea.left + (clientArea.right - clientArea.left) / 2;
    center.y = clientArea.top + (clientArea.bottom - clientArea.top) / 2;

    if (ClientToScreen(hwnd, &center))
    {
        this->neutralPosition = center;
    }
}

void MouseManager::RetrieveNeutralPosition(LPPOINT pos)
{
    pos->x = this->neutralPosition.x;
    pos->y = this->neutralPosition.y;
}

void MouseManager::SetNeutral()
{
    TrueApi::api.user32.SetCursorPos(this->neutralPosition.x, this->neutralPosition.y);
}
