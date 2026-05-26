// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#pragma once

#include <cstdint>

struct BitmapInfo565
{
    BITMAPINFOHEADER bmiHeader;
    DWORD masks[3];
};


class BinkOverlayWindow
{
public:
    BinkOverlayWindow() = default;
    ~BinkOverlayWindow()
    {
        Destroy();
    }

    /* Create the overlay window. */
    void Create(HWND hParent);

    /* Destroy the overlay window. */
    void Destroy();

    /* Show the overlay window. */
    void Show();

    /* Hide the overlay window. */
    void Hide();

    /* Move the overlay to the top of the Z-order */
    void Jump();

    /* Reposition the overlay window on parent window move. */
    /* The RECT should be in screen coordinates. */
    void NotifyMove(const RECT& newPos);

    /* Returns true iff the overlay is shown. */
    bool IsVisible() const;

    /* Returns the handle to the overlay window. */
    HWND GetHwnd() const;

    /* Sets buffer to the internal overlay render buffer. */
    /* Returns true iff buffer points to the render buffer. */
    bool GetBuffer(uint8_t*& buffer);

    /* Returns the DC for the internal buffer. */
    /* Returns NULL if GetBuffer has not been called yet. */
    HDC GetBufferDC() const;

    /* Notify the overlay that it is time to render the next frame. */
    void Render();

private:
    static constexpr const char* className = "BinkOverlayWindowClass";
    static constexpr int bufferWidth = 640;
    static constexpr int bufferHeight = 480;
    static constexpr int bufferBpp = 16;

    // These are both magenta
    static constexpr COLORREF transparency = RGB(255, 0, 255);
    static constexpr uint16_t transparency565 = 0xF81F;

    HDC bufferHdc{ nullptr };
    HBITMAP bufferBitmap{ nullptr };
    void* dibBits{ nullptr };

    HWND hwnd{ nullptr };

    bool visible{};

    void EraseBuffer();

    static LRESULT CALLBACK WndProc(
        HWND hwnd,
        UINT msg,
        WPARAM wParam,
        LPARAM lParam);
};

