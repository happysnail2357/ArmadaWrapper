// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#pragma once

class MouseManager
{
private:

    bool captured{ true };
    POINT neutralPosition{};

    /* Unhide the mouse cursor. */
    void ShowMouse();

    /* Hide the mouse cursor. */
    void HideMouse();

public:

    /* Allow full control of the mouse to the game. */
    /* Returns false if the mouse was already captured. */
    bool Capture();

    /* Allow the game to use the real mouse cursor for menus */
    /* as long as it doesn't hide or move the mouse around. */
    /* (This is the default behaviour) */
    void Share();

    /* Prevent the game from interefering with normal mouse operations. */
    void Release();

    /* Sends a mouse click release message to the window. */
    void Unclick(HWND hwnd);

    /* Returns true iff the mouse is captured by the game. */
    bool IsCaptured();

    /* This must be called whenever the window moves or resizes. */
    void UpdateNeutralPosition(HWND hwnd);

    /* This must be called whenever the window moves or resizes. */
    /* clientArea should be in **client** units NOT screen units. */
    void UpdateNeutralPosition(HWND hwnd, RECT clientArea);

    /* Sets a POINT with the location of the neutral position. */
    void RetrieveNeutralPosition(LPPOINT pos);

    /* Moves the mouse cursor to the game's neutral position (no game cursor movement). */
    void SetNeutral();
};

