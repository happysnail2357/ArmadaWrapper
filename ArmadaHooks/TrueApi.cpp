// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#include "pch.h"
#include "TrueApi.h"

TrueApi::ApiTrampolines TrueApi::api{};

void TrueApi::ConfigureTrampolines(void** trampolineList)
{
    if (trampolineList != nullptr)
    {
        void** apiStruct = reinterpret_cast<void**>(&TrueApi::api);

        int current = 0;
        while (trampolineList[current] != nullptr)
        {
            if (current < TrueApi::count)
            {
                apiStruct[current] = trampolineList[current];
            }

            current++;
        }

        if (current < TrueApi::count)
        {
            DEBUG_PRINTF(TEXT("ArmadaHooks - ConfigureTrueAPI received only %d of %d APIs"), current, TrueApi::count);
            ExitProcess(2);
        }
        else if (current > TrueApi::count)
        {
            DEBUG_PRINTF(TEXT("ArmadaHooks - ConfigureTrueAPI received %d APIs instead of %d"), current, TrueApi::count);
            ExitProcess(2);
        }
    }
    else
    {
        DEBUG_PRINT(TEXT("ArmadaHooks - ConfigureTrueAPI received nullptr instead of API list"));
        ExitProcess(2);
    }
}


// Exported C interface for TrueApi::ConfigureTrampolines
void __stdcall ConfigureTrueAPI(void** apiList)
{
    TrueApi::ConfigureTrampolines(apiList);
}

