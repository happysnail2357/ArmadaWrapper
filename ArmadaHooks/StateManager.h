// SPDX-License-Identifier: MIT
// Copyright © 2026 Paul Puhnaty

#pragma once

enum class DialogContext
{
    MainMenu,
    SinglePlayerMenu,
    MultiplayerMenu,
    OptionsMenu,
    GraphicsSettingsMenu,
    SoundSettingsMenu,
    GameSettingsMenu,
    SaveGameMenu,
    LoadSavedGameMenu,
    PauseMenu,
    DebriefMenu,
    NetworkGameSetupMenu,
    MultiplayerGameSetupMenu,
    MissionSelectPopup,
    GameCreatePopup,
    GameJoinPopup,
    AbortMissionPopup,
    MapEditorPopup,
    UnspecifiedPopup,
    Unknown,
};

enum class MenuAnimation
{
    None,
    Singleplayer,
    Multiplayer,
    Contruction,
    Wormhole,
    Federation,
    Klingon,
    Romulan,
    Borg,
    Omega,
};

class StateManager
{
public:

    /* Returns true if the dialog context represents a popup instead of a menu screen. */
    static bool DialogIsPopup(DialogContext context);

private:

    /* Flag to indicate main gameplay. */
    /* Default to true just in case the game is started in map editor mode. */
    bool inGame{ true };

    /* Flag to indicate that the user has chosen to abort the current mission. */
    bool abortMissionFlag{ false };

    /* Flag to indicate that the game is running in map editor mode. */
    /* Assume the game is running in map editor mode until the first dialog is created. */
    bool mapEditorMode{ true };

    /* While debugging the disassembly I found that */
    /* the dialog menus place values at this address */
    /* depending on which dialog button is pressed. */
    const DWORD* dialogResultFlag = reinterpret_cast<DWORD*>(0x68B8C0);

    /* Check the dialog parameters against a known list to identify it. */
    DialogContext DetermineDialogContext(ATOM templateId, DLGPROC lpDialogFunc);

public:

    /* Stores the animation over which a text message should be rendered. */
    /* See "BinkCopyToBuffer" and "WrapDrawTextA" */
    MenuAnimation textRenderTarget{};

    /* Report a dialog creation for internal state tracking. */
    /* Returns the context of dialog in the game. */
    DialogContext ReportDialogCreation(ATOM templateId, DLGPROC lpDialogFunc);

    /* Report a dialog closing for internal state tracking. */
    void ReportDialogClose(DialogContext context, INT_PTR result);

    /* Returns true if the game is currently in the main gameplay state. */
    bool IsInGame() const;

    /* Returns true if the game was started in map editor mode. */
    bool IsMapEditorMode() const;
};
