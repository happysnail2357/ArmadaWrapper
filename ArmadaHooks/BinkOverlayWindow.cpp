// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#include "pch.h"
#include "BinkOverlayWindow.h"


void BinkOverlayWindow::Create(HWND hParent)
{
    if (this->hwnd) return;

    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = &BinkOverlayWindow::WndProc;
    wc.hInstance = GetModuleHandleA(nullptr);
    wc.lpszClassName = BinkOverlayWindow::className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    //wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    RegisterClassExA(&wc);

    this->hwnd = CreateWindowExA(
        WS_EX_TOPMOST |
        WS_EX_TOOLWINDOW |
        WS_EX_LAYERED |
        WS_EX_TRANSPARENT,
        BinkOverlayWindow::className,
        "BinkOverlayWindow",
        WS_POPUP,
        0,
        0,
        1,
        1,
        hParent,
        nullptr,
        GetModuleHandleA(nullptr),
        this
    );

    if (this->hwnd)
    {
        // Black becomes transparent.
        SetLayeredWindowAttributes(
            this->hwnd,
            BinkOverlayWindow::transparency,
            255,
            LWA_COLORKEY
        );

        // Setup GDI resources

        HDC hdc = GetDC(this->hwnd);
        this->bufferHdc = CreateCompatibleDC(hdc);

        if (this->bufferBitmap)
        {
            SelectObject(this->bufferHdc, this->bufferBitmap);
        }

        ReleaseDC(this->hwnd, hdc);
    }
}

void BinkOverlayWindow::Destroy()
{
    if (this->hwnd)
    {
        DestroyWindow(this->hwnd);
        this->hwnd = nullptr;
        this->visible = false;

        if (this->bufferHdc)
        {
            // Release the device context but
            // leave the bitmap and buffer alone.

            DeleteDC(this->bufferHdc);
            this->bufferHdc = nullptr;
        }
    }
}

void BinkOverlayWindow::Show()
{
    if (this->hwnd && !this->visible)
    {
        this->visible = true;
        ShowWindow(this->hwnd, SW_SHOW);
        UpdateWindow(this->hwnd);
        this->Jump();
    }
}

void BinkOverlayWindow::Hide()
{
    if (this->hwnd && this->visible)
    {
        this->visible = false;
        ShowWindow(this->hwnd, SW_HIDE);
        EraseBuffer();
    }
}

void BinkOverlayWindow::Jump()
{
    if (!this->hwnd || !this->visible) return;

    SetWindowPos(
        this->hwnd,
        HWND_TOPMOST,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
    );
}

void BinkOverlayWindow::NotifyMove(const RECT& newPos)
{
    if (!this->hwnd) return;

    SetWindowPos(
        this->hwnd,
        HWND_TOPMOST,
        newPos.left,
        newPos.top,
        newPos.right - newPos.left,
        newPos.bottom - newPos.top,
        SWP_NOACTIVATE
    );
}

bool BinkOverlayWindow::IsVisible() const
{
    return this->visible;
}

HWND BinkOverlayWindow::GetHwnd() const
{
    return this->hwnd;
}

bool BinkOverlayWindow::GetBuffer(uint8_t*& buffer)
{
    if (this->bufferBitmap)
    {
        buffer = reinterpret_cast<uint8_t*>(dibBits);
        return true;
    }

    if (!this->hwnd)
        return false;

    HDC hdc = GetDC(this->hwnd);

    if (!this->bufferHdc)
        this->bufferHdc = CreateCompatibleDC(hdc);

    SetBkMode(this->bufferHdc, TRANSPARENT);
    SetTextColor(this->bufferHdc, RGB(255, 255, 255));

    BitmapInfo565 bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = BinkOverlayWindow::bufferWidth;
    bmi.bmiHeader.biHeight = -BinkOverlayWindow::bufferHeight; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = BinkOverlayWindow::bufferBpp;
    bmi.bmiHeader.biCompression = BI_BITFIELDS;

    bmi.masks[0] = 0xF800; // Red
    bmi.masks[1] = 0x07E0; // Green
    bmi.masks[2] = 0x001F; // Blue

    this->bufferBitmap = CreateDIBSection(
        hdc,
        reinterpret_cast<BITMAPINFO*>(&bmi),
        DIB_RGB_COLORS,
        &this->dibBits,
        nullptr,
        0
    );

    if (!this->bufferBitmap)
    {
        ReleaseDC(this->hwnd, hdc);
        return false;
    }

    EraseBuffer();

    SelectObject(bufferHdc, this->bufferBitmap);

    ReleaseDC(this->hwnd, hdc);

    buffer = reinterpret_cast<uint8_t*>(dibBits);

    return true;
}

HDC BinkOverlayWindow::GetBufferDC() const
{
    return this->bufferHdc;
}

void BinkOverlayWindow::Render()
{
    if (!this->visible || !this->bufferHdc || !this->bufferBitmap)
        return;

    HDC hdcWindow = GetDC(this->hwnd);

    BitBlt(
        hdcWindow,
        0, 0,
        BinkOverlayWindow::bufferWidth, BinkOverlayWindow::bufferHeight,
        this->bufferHdc,
        0, 0,
        SRCCOPY
    );

    ReleaseDC(this->hwnd, hdcWindow);
}


void BinkOverlayWindow::EraseBuffer()
{
    if (dibBits)
    {
        // The bitmap buffer uses 16 bit colors in the 565 RGB format

        uint16_t* pixels = reinterpret_cast<uint16_t*>(dibBits);

        for (int i = 0; i < BinkOverlayWindow::bufferWidth * BinkOverlayWindow::bufferHeight; i++)
        {
            pixels[i] = BinkOverlayWindow::transparency565;
        }
    }
}

LRESULT CALLBACK BinkOverlayWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_ERASEBKGND:
    {
        HDC hdc = reinterpret_cast<HDC>(wParam);

        RECT rc;
        GetClientRect(hwnd, &rc);

        HBRUSH brush = CreateSolidBrush(BinkOverlayWindow::transparency);

        FillRect(hdc, &rc, brush);

        DeleteObject(brush);

    } return 1;

    case WM_NCHITTEST:
        // Fully click-through.
        return HTTRANSPARENT;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}
