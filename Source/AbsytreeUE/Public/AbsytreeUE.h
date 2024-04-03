#pragma once

#include "CoreMinimal.h"
#include "EditorWrapper.h"
#include "HAL/Platform.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class FAbsytreeUEModule;

extern FAbsytreeUEModule* GAbsytreeUEModule;

class FAbsytreeUEModule : public IModuleInterface
{
public:

    // - IModuleInterface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    // --

    void Tick(float DeltaTime);

private:
    void PluginButtonClicked();

    void RegisterMenus();
    void ReloadDll() const;

    TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

    float TimeAccumulator = 0;

    TSharedPtr<class FUICommandList> PluginCommands;
    TSharedPtr<FEditorWrapper> EditorWrapper;
};
