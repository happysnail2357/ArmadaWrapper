// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#include "pch.h"
#include "CustomDialogBox.h"

std::list<DialogHandle*> CustomDialogBox::dialogHandles{};

void CustomDialogBox::NotifyMove(const RECT& newPos)
{
    if (dialogHandles.empty()) return;

    auto handle = dialogHandles.back();

    if (handle->properties.isPopup) return;

    SetWindowPos(
        handle->window, // handle to the dialog
        NULL,           // no Z-order change
        newPos.left,                // new left position
        newPos.top,                 // new top position
        newPos.right - newPos.left, // width
        newPos.bottom - newPos.top, // height
        SWP_NOZORDER
    );
}

bool CustomDialogBox::NotifyClose(HWND hDlg, INT_PTR result)
{
    for (DialogHandle* handle : dialogHandles)
    {
        if (handle->window == hDlg)
        {
            handle->result = result;
            handle->closeFlag = true;
            return true;
        }
    }

    return false;
}

bool CustomDialogBox::IsCustomDialog(HWND hDlg)
{
    for (DialogHandle* handle : dialogHandles)
    {
        if (handle->window == hDlg)
        {
            return true;
        }
    }

    return false;
}

bool CustomDialogBox::ActivateTopmost()
{
    if (dialogHandles.empty()) return false;

    auto handle = dialogHandles.back();

    return SetWindowPos(
        handle->window,
        0, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER
    );
}


bool CustomDialogBox::Create()
{
    this->handle.window = CreateDialogParamA(
        hInstance,
        lpTemplateName,
        hWndParent,
        lpDialogFunc,
        dwInitParam
    );

    if (this->handle.window == nullptr)
    {
        DEBUG_PRINTF(TEXT("Dialog creation failed with system error code %d"), GetLastError());
        return false;
    }

    dialogHandles.push_back(&(this->handle));
    return true;
}

void CustomDialogBox::RunMessageLoop()
{
    MSG msg;
    BOOL getMessageResult;

    while (getMessageResult = GetMessage(&msg, NULL, 0, 0))
    {
        if (getMessageResult < 0)
        {
            DEBUG_PRINT(TEXT("CustomDialogBox: Unable to retrieve next message!"));
        }

        if (!IsDialogMessage(this->handle.window, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!IsWindow(this->handle.window) || this->handle.closeFlag)
        {
            break;
        }
    }
}

void CustomDialogBox::Destroy()
{
    auto it = std::find(dialogHandles.begin(), dialogHandles.end(), &(this->handle));

    if (it != dialogHandles.end())
    {
        dialogHandles.erase(it);
    }

    DestroyWindow(this->handle.window);
    this->handle.window = NULL;
    this->handle.closeFlag = false;
}


CustomDialogBox::CustomDialogBox(
    HINSTANCE hInstance,
    LPCSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam
) :
    hInstance(hInstance),
    lpTemplateName(lpTemplateName),
    hWndParent(hWndParent),
    lpDialogFunc(lpDialogFunc),
    dwInitParam(dwInitParam)
{
    uintptr_t templateNameAsInt = reinterpret_cast<uintptr_t>(lpTemplateName);

    if (templateNameAsInt <= 0xFFFF)
    {
        this->templateId = static_cast<ATOM>(templateNameAsInt);
    }
    else
    {
        this->templateId = 0;
        DEBUG_PRINT(TEXT("CustomDialogBox: Template name is not an ATOM!"));
    }
}

INT_PTR CustomDialogBox::Run(DialogProperties& properties)
{
    this->handle.result = -1;
    this->handle.properties = properties;

    if (!this->Create())
    {
        return this->handle.result;
    }

    if (properties.restrictKeyboardFocus)
    {
        SetFocus(NULL);
    }

    ShowWindow(this->handle.window, SW_SHOW);

    if (properties.setTopmost)
    {
        SetWindowPos(
            this->handle.window,
            HWND_TOPMOST,
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
        );
    }

    if (dialogHandles.size() > 1)
    {
        auto it = dialogHandles.rbegin(); // this dialog
        it++; // previous dialog

        if (properties.isPopup)
        {
            // Disable previous dialog
            EnableWindow((*it)->window, FALSE);
        }
        else
        {
            // Hide previous dialog
            ShowWindow((*it)->window, SW_HIDE);
        }
    }

    this->RunMessageLoop();
    this->Destroy();

    if (!dialogHandles.empty())
    {
        auto handle = dialogHandles.back();

        if (properties.isPopup)
        {
            // Re-enable the previous dialog
            EnableWindow(handle->window, TRUE);
            SetActiveWindow(handle->window);
        }
        else
        {
            // Unhide the previous dialog
            ShowWindow(handle->window, SW_SHOW);
        }
    }

    return this->handle.result;
}

ATOM CustomDialogBox::Id() const
{
    return this->templateId;
}
