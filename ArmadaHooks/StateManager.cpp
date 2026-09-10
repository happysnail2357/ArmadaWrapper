// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#include "pch.h"
#include "StateManager.h"


bool StateManager::DialogIsPopup(DialogContext context)
{
    switch (context)
    {
    case DialogContext::MissionSelectPopup:
    case DialogContext::MapSelectPopup:
    case DialogContext::GameCreatePopup:
    case DialogContext::GamePasswordPopup:
    case DialogContext::AbortMissionPopup:
    case DialogContext::MapEditorPopup:
    case DialogContext::UnspecifiedPopup:
        return true;

    default:
        return false;
    }
}


DialogContext StateManager::DetermineDialogContext(ATOM templateId, DLGPROC lpDialogFunc)
{
    switch (templateId)
    {
    case 0x001:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x00543330:
            return DialogContext::GraphicsSettingsMenu;

        default:
            return DialogContext::Unknown;
        }

    case 0x065:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x00496cd0:
            return DialogContext::MapEditorPopup;

        default:
            return DialogContext::Unknown;
        }

    case 0x066:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x004956b0:
            return DialogContext::MapEditorPopup;

        default:
            return DialogContext::Unknown;
        }

    case 0x073:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x005499c0:
            return DialogContext::SinglePlayerMenu;

        case 0x0054c230:
            return DialogContext::AbortMissionPopup;

        case 0x0053c5d0:
        case 0x00547940:
            return DialogContext::UnspecifiedPopup;

        default:
            return DialogContext::Unknown;
        }

    case 0x123:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x00540530:
            return DialogContext::OptionsMenu;

        case 0x0054adc0:
            return DialogContext::SoundSettingsMenu;

        case 0x00541940:
            return DialogContext::GameSettingsMenu;

        case 0x00544d90:
            return DialogContext::LoadSavedGameMenu;

        default:
            return DialogContext::MainMenu;
        }

    case 0x124:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x00547020:
            return DialogContext::MissionSelectPopup;

        default:
            return DialogContext::Unknown;
        }

    case 0x125:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x00497270:
            return DialogContext::MapEditorPopup;

        default:
            return DialogContext::Unknown;
        }

    case 0x128:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x00496340:
            return DialogContext::MapEditorPopup;

        default:
            return DialogContext::Unknown;
        }

    case 0x129:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x00548330:
            return DialogContext::SaveGameMenu;

        default:
            return DialogContext::Unknown;
        }

    case 0x12d:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x00494c70:
            return DialogContext::MapEditorPopup;

        default:
            return DialogContext::Unknown;
        }

    case 0x830:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x00536250:
            return DialogContext::MultiplayerGameSetupMenu;

        default:
            return DialogContext::Unknown;
        }

    case 0x871:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x0052c5c0:
            return DialogContext::NetworkGameSetupMenu;

        default:
            return DialogContext::Unknown;
        }

    case 0x872:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x00532b80:
            return DialogContext::MultiplayerMenu;

        default:
            return DialogContext::Unknown;
        }

    case 0x873:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x0052b2b0:
            return DialogContext::GameCreatePopup;

        default:
            return DialogContext::Unknown;
        }

    case 0x874:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x0053cbc0:
            return DialogContext::GamePasswordPopup;

        default:
            return DialogContext::Unknown;
        }

    case 0x87D:
        switch (reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x00534bb0:
            return DialogContext::DebriefMenu;

        default:
            return DialogContext::Unknown;
        }

    case 0x87F:
        switch(reinterpret_cast<int>(lpDialogFunc))
        {
        case 0x00530b70:
            return DialogContext::MapSelectPopup;

        default:
            return DialogContext::Unknown;
        }
        
    default:
        return DialogContext::Unknown;
    }
}


DialogContext StateManager::ReportDialogCreation(ATOM templateId, DLGPROC lpDialogFunc)
{
    DialogContext context = DetermineDialogContext(templateId, lpDialogFunc);

    if (context == DialogContext::Unknown)
    {
        DEBUG_PRINTF(TEXT("Unknown dialog: Template Id = 0x%04x, Procedure Address = 0x%08x"), templateId, reinterpret_cast<int>(lpDialogFunc));
    }
    else if (context == DialogContext::MainMenu)
    {
        this->inGame = false;
    }
    else if (context == DialogContext::MapEditorPopup)
    {
        this->usingMapEditTools = true;
    }

    if (this->inGame)
    {
        // If a dialog box is being created,
        // then the game is either paused or ended
        this->inGame = false;

        if (context == DialogContext::OptionsMenu)
        {
            context = DialogContext::PauseMenu;
        }
    }

    return context;
}

void StateManager::ReportDialogClose(DialogContext context, INT_PTR result)
{
    // The dialog context and result value are used to determine
    // which screen the game is on. We need to do this so we 
    // know when the game transitions to the "in game" RTS screen.

    if (result == 1)
    {
        // Multiplayer setup and single player select dialogs
        if (context == DialogContext::MultiplayerGameSetupMenu ||
            context == DialogContext::MissionSelectPopup)
        {
            this->inGame = true;
            this->textRenderTarget = MenuAnimation::None;
        }
        else if (context == DialogContext::PauseMenu)
        {
            // User chose:
            // - "Return to Game" or
            // - "Restate Objectives" or
            // - "Abort Mission"
            if (*dialogResultFlag == 0x0)
            {
                if (this->abortMissionFlag)
                {
                    // Clear the flag for next time
                    this->abortMissionFlag = false;
                }
                else
                {
                    this->inGame = true;
                }
            }
        }
        else if (context == DialogContext::LoadSavedGameMenu)
        {
            // User chose "Load Game"
            if (*dialogResultFlag == 0x0)
            {
                this->inGame = true;
            }
        }
        else if (context == DialogContext::AbortMissionPopup)
        {
            this->abortMissionFlag = true;
        }
    }

    if (context == DialogContext::MapEditorPopup)
    {
        this->inGame = true;
        this->usingMapEditTools = false;
    }
}

bool StateManager::IsInGame() const
{
    return this->inGame;
}

bool StateManager::UsingMapEditor() const
{
    return this->usingMapEditTools;
}
