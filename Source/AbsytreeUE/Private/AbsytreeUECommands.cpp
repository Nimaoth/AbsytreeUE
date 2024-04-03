// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbsytreeUECommands.h"

#define LOCTEXT_NAMESPACE "FAbsytreeUEModule"

void FAbsytreeUECommands::RegisterCommands()
{
    UI_COMMAND(OpenPluginWindow, "AbsytreeUE", "Bring up AbsytreeUE window", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(ReloadDll, "Reload Absytree DLL", "Reload AbsytreeUE dll", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
