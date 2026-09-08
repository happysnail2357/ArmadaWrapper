// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#include "pch.h"
#include "Binkw32Wrap.h"
#include "HookAssets.h"

#include <unordered_map>
#include <string>


struct HoverableMenuAnimation
{
    MenuAnimation animation;
    bool hovered;
};

static const std::unordered_map<std::string, const HoverableMenuAnimation> binkFiles
{
    {
        "animations\\single01.bik",
        { MenuAnimation::Singleplayer, false }
    },
    {
        "animations\\single02.bik",
        { MenuAnimation::Singleplayer, true }
    },
    {
        "animations\\multi01.bik",
        { MenuAnimation::Multiplayer, false }
    },
    {
        "animations\\multi02.bik",
        { MenuAnimation::Multiplayer, true }
    },
    {
        "animations\\const01.bik",
        { MenuAnimation::Contruction, false }
    },
    {
        "animations\\const02.bik",
        { MenuAnimation::Contruction, true }
    },
    {
        "animations\\credit01.bik",
        { MenuAnimation::Wormhole, false }
    },
    {
        "animations\\credit02.bik",
        { MenuAnimation::Wormhole, true }
    },
    {
        "animations\\fed01.bik",
        { MenuAnimation::Federation, false }
    },
    {
        "animations\\fed02.bik",
        { MenuAnimation::Federation, true }
    },
    {
        "animations\\kling01.bik",
        { MenuAnimation::Klingon, false }
    },
    {
        "animations\\kling02.bik",
        { MenuAnimation::Klingon, true }
    },
    {
        "animations\\rom01.bik",
        { MenuAnimation::Romulan, false }
    },
    {
        "animations\\rom02.bik",
        { MenuAnimation::Romulan, true }
    },
    {
        "animations\\borg01.bik",
        { MenuAnimation::Borg, false }
    },
    {
        "animations\\borg02.bik",
        { MenuAnimation::Borg, true }
    },
    {
        "animations\\omega01.bik",
        { MenuAnimation::Omega, false }
    },
    {
        "animations\\omega02.bik",
        { MenuAnimation::Omega, true }
    },
};

static std::unordered_map<HBINK, HoverableMenuAnimation> binkHandles{};

static uint8_t* overlayBuffer{ nullptr };


extern "C" __declspec(dllexport)
HBINK __stdcall BinkOpen(const char* filename, unsigned int flags)
{
    HBINK handle = TrueApi::api.binkw32.BinkOpen(filename, flags);

    auto mappedMovie = binkFiles.find(filename);

    if (mappedMovie != binkFiles.end())
    {
        binkHandles.insert({ handle, mappedMovie->second });

        if (overlayBuffer == nullptr)
        {
            HookAssets::binkWindow.GetBuffer(overlayBuffer);
        }
    }

    return handle;
}

extern "C" __declspec(dllexport)
void __stdcall BinkClose(HBINK bnk)
{
    binkHandles.erase(bnk);

    TrueApi::api.binkw32.BinkClose(bnk);
}

extern "C" __declspec(dllexport)
void __stdcall BinkWait(HBINK bnk)
{
    HookAssets::binkWindow.Render();

    TrueApi::api.binkw32.BinkWait(bnk);
}


extern "C" __declspec(dllexport)
void __stdcall BinkCopyToBuffer(
    HBINK bnk,
    void* dest,
    unsigned int destPitch,
    unsigned int destHeight,
    unsigned int destx,
    unsigned int desty,
    unsigned int flags)
{
    auto handle = binkHandles.find(bnk);
    if (handle != binkHandles.end())
    {
        if (overlayBuffer)
        {
            // If no (x, y) coordinates are given, then this is going to be
            // an animation with the "<Race> campaign not complete" message
            // on top. We'll just redirect this copy into our buffer like
            // the normal animations and handle the text in WrapDrawTextA.
            if (!destx && !desty && handle->second.hovered)
            {
                switch (handle->second.animation)
                {
                case MenuAnimation::Klingon:
                    destx = 360;
                    desty = 30;
                    destPitch = 1280;
                    break;

                case MenuAnimation::Romulan:
                    destx = 0;
                    desty = 250;
                    destPitch = 1280;
                    break;

                case MenuAnimation::Borg:
                    destx = 360;
                    desty = 250;
                    destPitch = 1280;
                    break;

                default:
                    DEBUG_PRINTF(TEXT("Bink frame copied to origin! MenuAnimation #%d%s"), handle->second.animation, handle->second.hovered ? TEXT(" hovered") : TEXT(""));
                }

                HookAssets::state.textRenderTarget = handle->second.animation;
            }
            else if (handle->second.animation == HookAssets::state.textRenderTarget)
            {
                HookAssets::state.textRenderTarget = MenuAnimation::None;
            }

            TrueApi::api.binkw32.BinkCopyToBuffer(bnk, overlayBuffer, destPitch, destHeight, destx, desty, flags);
            return;
        }
    }

    TrueApi::api.binkw32.BinkCopyToBuffer(bnk, dest, destPitch, destHeight, destx, desty, flags);
}
