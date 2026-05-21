// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#include "pch.h"
#include <vector>

typedef void (WINAPI* configAPI_t)(void**);

struct LibraryData
{
    HMODULE handle;
    const char* name;
};

struct WrapperData
{
    LibraryData library
    {
        nullptr,
        "armadawrap.dll"
    };
    std::vector<void*> trampolines;
};


// This file is auto-generated during build
#include "ApiTables.inc"


static void HookFunctions(LibraryData& library, const char** apiList, WrapperData& wrapper)
{
    for (int i = 0; apiList[i]; i++)
    {
        const char* exportName = apiList[i];

        FARPROC original = GetProcAddress(library.handle, exportName);
        FARPROC replacement = GetProcAddress(wrapper.library.handle, exportName);

        if (original && replacement)
        {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&(PVOID&)original, replacement);
            DetourTransactionCommit();
            wrapper.trampolines.push_back(static_cast<void*>(original));
        }

        if (!original)
        {
            DEBUG_PRINTF_A("Failed to load \"%s\" from \"%s\"", exportName, library.name);
        }
        
        if (!replacement)
        {
            DEBUG_PRINTF_A("Failed to load \"%s\" from \"%s\"", exportName, wrapper.library.name);
        }
    }
}

static void HookLibrary(const char* libraryName, const char** apiList, WrapperData& wrapper)
{
    LibraryData library
    {
        GetModuleHandleA(libraryName),
        libraryName
    };

    if (!library.handle)
    {
        DEBUG_PRINTF_A("Unable to get module handle for \"%s\"", library.name);
        library.handle = LoadLibraryA(library.name);
    }

    if (library.handle)
    {
        HookFunctions(library, apiList, wrapper);
        DEBUG_PRINTF_A("Hooked \"%s\" with \"%s\"", library.name, wrapper.library.name);
    }
    else
    {
        DEBUG_PRINTF_A("Unable to load \"%s\"", library.name);
    }
}

void InitializeWrapper()
{
    WrapperData wrapper{};

    wrapper.library.handle = LoadLibraryA(wrapper.library.name);

    if (wrapper.library.handle)
    {
        configAPI_t trampolineConfig = reinterpret_cast<configAPI_t>(GetProcAddress(wrapper.library.handle, "ConfigureTrueAPI"));

        if (trampolineConfig)
        {
            // This file is auto-generated during build
            #include "HookLibraryCalls.inc"

            // "ConfigureTrueAPI" requires the trampoline
            // list to be a null terminated array.
            wrapper.trampolines.push_back(nullptr);

            trampolineConfig(wrapper.trampolines.data());
        }
        else
        {
            DEBUG_PRINTF_A("\"%s\" does not export \"ConfigureTrueAPI\"", wrapper.library.name);
        }
    }
    else
    {
        DEBUG_PRINTF_A("Unable to load \"%s\"", wrapper.library.name);
    }
}
