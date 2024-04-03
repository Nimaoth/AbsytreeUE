// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "AbsytreeUEStyle.h"

class FAbsytreeUECommands : public TCommands<FAbsytreeUECommands>
{
public:
    FAbsytreeUECommands()
        : TCommands<FAbsytreeUECommands>(TEXT("AbsytreeUE"), NSLOCTEXT("Contexts", "AbsytreeUE", "AbsytreeUE Plugin"),
                                         NAME_None, FAbsytreeUEStyle::GetStyleSetName())
    {
    }

    // TCommands<> interface
    virtual void RegisterCommands() override;

public:
    TSharedPtr<FUICommandInfo> OpenPluginWindow;
    TSharedPtr<FUICommandInfo> ReloadDll;
};
