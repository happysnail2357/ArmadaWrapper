// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#pragma once

typedef struct BINK
{
    unsigned int Width;
    unsigned int Height;

} BINK, * HBINK;

extern "C" __declspec(dllexport)
HBINK __stdcall BinkOpen(const char* filename, unsigned int flags);

extern "C" __declspec(dllexport)
void __stdcall BinkClose(HBINK bnk);

extern "C" __declspec(dllexport)
void __stdcall BinkWait(HBINK bnk);

extern "C" __declspec(dllexport)
void __stdcall BinkCopyToBuffer(
    HBINK bnk,
    void* dest,
    unsigned int destpitch,
    unsigned int destheight,
    unsigned int destx,
    unsigned int desty,
    unsigned int flags);
