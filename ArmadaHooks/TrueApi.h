// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#pragma once

#include "Binkw32Wrap.h"

class TrueApi
{
public:

    struct ApiTrampolines
    {
        // This file is auto-generated during build
        #include "HookedApiDeclarations.inc"
    };

    static const int count = sizeof(ApiTrampolines) / sizeof(void*);

    static ApiTrampolines api;

    static void ConfigureTrampolines(void** trampolines);
};

