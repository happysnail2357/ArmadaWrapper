# Developer Information

ArmadaWrapper is built with Microsoft Visual Studio 2022 and is comprised of two DLL projects.
Python is required for the build process.

## Project Overview

ArmadaWrapper works by hooking DLLs used by the game. The *WinmmProxy* project creates
a proxy `winmm.dll` that hijacks the DLL load order when placed in the game folder.
This DLL forwards all the multimedia APIs exported by the real WinMM library, but
it also uses the [Microsoft Detours](https://www.microsoft.com/en-us/research/project/detours/)
library to inject ArmadaWrapper code in the place of other APIs.
(See [apihooks.map](./WinmmProxy/apihooks.map) for a list of targeted APIs)

The *ArmadaHooks* project contains the interesting code that actually manipulates the game.
Several different techniques are used to improve the game experience including
modifying API calls, intercepting window messages, and even replacing an API.

## Build Information

The Detours library is handled by [vcpkg](https://vcpkg.io/en/package/detours) and
should automatically download on the first build attempt.

In the current design, the *WinmmProxy* DLL must provide the *ArmadaHooks* DLL with a list of 
API trampolines at runtime. When adding and removing from the list of targeted APIs, this 
transfer can become fragile. To ensure that both DLLs are working with the same set of APIs, a
single [API list](./WinmmProxy/apihooks.map) is used to auto generate the code that manages
this process (see [stub_generator.py](./WinmmProxy/stub_generator.py)). This step requires Python to be
installed and will automatically run during build.

A Powershell [script](./applychanges.ps1) is provided to copy the build output to game folder.
(Note: this script assumes you installed the game in its default directory,
which will require the script to have admin privileges to work)
