// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#include "pch.h"
#include "CustomDialogBox.h"

std::list<DialogHandle*> CustomDialogBox::dialogHandles{};

void CustomDialogBox::NotifyMove(const RECT& newPos)
{
    if (dialogHandles.empty()) return;

    auto handle = dialogHandles.back();

    if (handle->popup) return;

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


void CustomDialogBox::Create()
{
    HRSRC hRes = FindResourceA(this->hInstance, this->lpTemplateName, RT_DIALOG_A);
    if (!hRes) throw std::exception("Unable to find resource");

    HGLOBAL hGlobal = LoadResource(hInstance, hRes);
    if (!hGlobal) throw std::exception("Unable to load resource");

    BYTE* pOrigData = static_cast<BYTE*>(LockResource(hGlobal));
    if (!pOrigData) throw std::exception("Unable to lock resource");

    SIZE_T templateSize = SizeofResource(hInstance, hRes);
    BYTE* pNewData = new BYTE[templateSize];

    memcpy(pNewData, pOrigData, templateSize);

    // Determine if this is a DLGTEMPLATEEX
    WORD dlgVer = *reinterpret_cast<WORD*>(pNewData);
    WORD signature = *reinterpret_cast<WORD*>(pNewData + 2);

    if (dlgVer == 1 && signature == 0xFFFF)
    {
        // It's a DLGTEMPLATEEX
        DLGTEMPLATEEX* pDlgEx = reinterpret_cast<DLGTEMPLATEEX*>(pNewData);

        //DEBUG_PRINTF(TEXT("Dialog style: 0x%x"), pDlgEx->style);
        //DEBUG_PRINTF(TEXT("Dialog exStyle: 0x%x"), pDlgEx->exStyle);

        // Modify the style: remove WS_POPUP, add WS_CHILD
        //pDlgEx->style &= ~WS_POPUP;
        //pDlgEx->style |= WS_CHILD;
        
        this->handle.window = CreateDialogIndirectParamA(
            hInstance,
            reinterpret_cast<DLGTEMPLATE*>(pNewData),  // cast is OK: both are memory-compatible
            hWndParent,
            lpDialogFunc,
            dwInitParam
        );
    }
    else
    {
        // It's a standard DLGTEMPLATE
        DLGTEMPLATE* pDlg = reinterpret_cast<DLGTEMPLATE*>(pNewData);

        //pDlg->style &= ~WS_POPUP;
        //pDlg->style |= WS_CHILD;

        this->handle.window = CreateDialogIndirectParamA(
            hInstance,
            pDlg,
            hWndParent,
            lpDialogFunc,
            dwInitParam
        );
    }

    delete[] pNewData;

    if (this->handle.window == nullptr)
    {
        throw std::exception("Dialog creation failed");
    }

    dialogHandles.push_back(&(this->handle));

    //DEBUG_PRINTF(TEXT("Create dialog: 0x%08x"), reinterpret_cast<uintptr_t>(this->handle.window));
}

void CustomDialogBox::RunMessageLoop()
{
    MSG msg;
    BOOL getMessageResult;

    while (getMessageResult = GetMessage(&msg, NULL, 0, 0))
    {
        if (getMessageResult < 0)
        {
            DEBUG_PRINTF(TEXT("Unable to retrieve next message!"));
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

    //DEBUG_PRINTF(TEXT("Destroying dialog: 0x%08x"), reinterpret_cast<uintptr_t>(this->handle.window));

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

INT_PTR CustomDialogBox::Run(bool popup)
{
    this->handle.result = -1;

    try
    {
        this->Create();
    }
    catch (const std::exception& e)
    {
        DEBUG_PRINTF_A("Dialog creation failed: %s", e.what());
        return this->handle.result;
    }

    this->handle.popup = popup;

    SetFocus(NULL);
    ShowWindow(this->handle.window, SW_SHOW);
    UpdateWindow(this->handle.window);

    if (dialogHandles.size() > 1)
    {
        auto it = dialogHandles.rbegin(); // this dialog
        it++; // previous dialog

        if (popup)
        {
            // Disable previous dialog
            EnableWindow((*it)->window, FALSE);
            SetWindowPos(
                this->handle.window,
                HWND_TOPMOST,
                0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
            );
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

        if (popup)
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
