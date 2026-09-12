# Armada Installation and Setup on Windows 10/11

1. [Install Armada](#step-1---install-armada)
1. [Install Armada Patches](#step-2---install-armada-patches)
1. [Install ArmadaWrapper](#step-3---install-armadawrapper)
1. [Install dgVoodoo2](#step-4---install-dgvoodoo2)
1. [Configure Armada Settings](#step-5---configure-armada-settings)
1. [Additional Information](#additional-information)

## Step 1 - Install Armada

Run the Armada setup program and install the game normally.

> [!NOTE]
> Remember the folder you install the game in.
> You will need to copy files there later.
> The default location is `C:\Program Files (x86)\Activision\Star Trek - Armada\`.

Close the setup once it has finished.

:construction: **Don't try to open the game until Step 4 is complete!** :construction:

## Step 2 - Install Armada Patches

Download the [official Armada 1.2 patch](http://armadafiles.com/files/armada/official-releases/patches/star-trek-armada-12-patch/details)
and install it. (You can skip this if your copy of Armada is already version 1.2)

Download the [unofficial Armada 1.3 patch](http://armadafiles.com/files/armada/mods/patches/star-trek-armada-patch-1-3-project/details)
and install it.

## Step 3 - Install ArmadaWrapper

Download the [latest release](https://github.com/happysnail2357/ArmadaWrapper/releases/latest) of ArmadaWrapper.
Extract the downloaded zip archive into the game's folder. It contains three files: `winmm.dll`, `armadawrap.dll`, and `dgVoodoo.conf`.

## Step 4 - Install dgVoodoo2

Download the latest stable version of the [dgVoodoo2 wrapper](https://www.dege.freeweb.hu/dgVoodoo2/dgVoodoo2/).
Extract the files from the downloaded zip archive into a temporary folder.
Inside that folder you should find a folder called `MS` and inside that folder should be another one called `x86`.
This folder should have four DLL files inside of it. Copy them into the game's folder.

## Step 5 - Configure Armada Settings

At this point you should be able to run Armada normally.

:no_entry_sign: **Don't apply any Windows compatibility settings to the game!** :no_entry_sign:

Once you have Armada running, goto the *Options* menu and then the *Graphics Settings* menu.
Set *Graphics Detail* to its maximum value. For *Display Mode* choose the resolution you prefer from the dropdown list:

- 640x480 - classic look, seamless menu transitions
- max 4:3 - original aspect ratio, best qualilty
- max 16:9 - widescreen, best qualilty
- max - highest resolution your monitor supports, best quality

## Additional Information

- The taskbar and start menu can be accessed in the game by pressing the Windows (super) key.
- The `Alt` key can be used to release and capture the mouse while playing a mission or multiplayer game.
- The Armada [map editor](http://armadafiles.com/files/armada/utilities/mapping-tools/armada-map-editor/details) also works great!
- This guide was inspired by this old [Reddit post](https://www.reddit.com/r/StarTrekArmada/comments/6jtok9/an_easy_guide_to_get_star_trek_armada_working_in/).
