// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#pragma once

#include "TrueApi.h"
#include "StateManager.h"
#include "MouseManager.h"
#include "BinkOverlayWindow.h"

class HookAssets
{
public:

    static StateManager state;
    static MouseManager mouse;
    static BinkOverlayWindow binkWindow;
};
