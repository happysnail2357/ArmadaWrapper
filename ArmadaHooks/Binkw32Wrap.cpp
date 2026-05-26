// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#include "pch.h"
#include "Binkw32Wrap.h"
#include "User32Wrap.h"
#include "TrueApi.h"

#include <unordered_map>
#include <string>


static const std::unordered_map<std::string, const BinkMovie> binkFiles
{
    {
        "animations\\single01.bik",
        BinkMovie::Singleplayer
    },
    {
        "animations\\single02.bik",
        BinkMovie::SingleplayerHover
    },
    {
        "animations\\multi01.bik",
        BinkMovie::Multiplayer
    },
    {
        "animations\\multi02.bik",
        BinkMovie::MultiplayerHover
    },
    {
        "animations\\const01.bik",
        BinkMovie::Contruction
    },
    {
        "animations\\const02.bik",
        BinkMovie::ContructionHover
    },
    {
        "animations\\credit01.bik",
        BinkMovie::Wormhole
    },
    {
        "animations\\credit02.bik",
        BinkMovie::WormholeHover
    },
    {
        "animations\\fed01.bik",
        BinkMovie::Federation
    },
    {
        "animations\\fed02.bik",
        BinkMovie::FederationHover
    },
    {
        "animations\\kling01.bik",
        BinkMovie::Klingon
    },
    {
        "animations\\kling02.bik",
        BinkMovie::KlingonHover
    },
    {
        "animations\\rom01.bik",
        BinkMovie::Romulan
    },
    {
        "animations\\rom02.bik",
        BinkMovie::RomulanHover
    },
    {
        "animations\\borg01.bik",
        BinkMovie::Borg
    },
    {
        "animations\\borg02.bik",
        BinkMovie::BorgHover
    },
    {
        "animations\\omega01.bik",
        BinkMovie::Omega
    },
    {
        "animations\\omega02.bik",
        BinkMovie::OmegaHover
    },
};

static std::unordered_map<HBINK, BinkMovie> binkHandles{};

static uint8_t* overlayBuffer{ nullptr };

extern BinkMovie hoveredTextTarget{};

/* Returns the non-hover equivalent to a hover bink movie. */
static BinkMovie NotHovered(BinkMovie hoverMovie)
{
    if (hoverMovie == BinkMovie::None) return BinkMovie::None;

    int movieAsInt = static_cast<int>(hoverMovie);

    if (movieAsInt % 2 == 1) return hoverMovie;

    return static_cast<BinkMovie>(movieAsInt - 1);
}


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
            binkWindow.GetBuffer(overlayBuffer);
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
    binkWindow.Render();

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
            if (!destx && !desty)
            {
                switch (handle->second)
                {
                case BinkMovie::KlingonHover:
                    destx = 360;
                    desty = 30;
                    destPitch = 1280;
                    break;

                case BinkMovie::RomulanHover:
                    destx = 0;
                    desty = 250;
                    destPitch = 1280;
                    break;

                case BinkMovie::BorgHover:
                    destx = 360;
                    desty = 250;
                    destPitch = 1280;
                    break;

                default:
                    DEBUG_PRINTF(TEXT("Bink frame copied to origin! BinkMovie #%d"), handle->second);
                }

                hoveredTextTarget = handle->second;
            }
            else if (handle->second == NotHovered(hoveredTextTarget))
            {
                hoveredTextTarget = BinkMovie::None;
            }

            TrueApi::api.binkw32.BinkCopyToBuffer(bnk, overlayBuffer, destPitch, destHeight, destx, desty, flags);
            return;
        }
    }

    TrueApi::api.binkw32.BinkCopyToBuffer(bnk, dest, destPitch, destHeight, destx, desty, flags);
}
