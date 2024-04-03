#include "AbsytreeUE.h"
#include "AbsytreeUEStyle.h"
#include "AbsytreeUECommands.h"
#include "Logging/LogMacros.h"
#include "SAbsytreeView.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "HAL/Runnable.h"
#include "Framework/Application/SlateApplication.h"

#include "AbsytreeUESettings.h"
#include "ISettingsModule.h"

static const FName AbsytreeUETabName("AbsytreeUE");

#define LOCTEXT_NAMESPACE "FAbsytreeUEModule"

DEFINE_LOG_CATEGORY(LogAbsytreeUE);

struct FAbsytreeThread : public FRunnable
{
    virtual uint32 Run() override;
};

uint32 FAbsytreeThread::Run()
{
    return 0;
}

FAbsytreeThread GAbsytreeThread;
FRunnableThread* GAbsytreeRunnableThread = nullptr;
FAbsytreeUEModule* GAbsytreeUEModule = nullptr;

void HandleSlateTick(float DeltaTime)
{
    if (GAbsytreeUEModule)
    {
        GAbsytreeUEModule->Tick(DeltaTime);
    }
}

void FAbsytreeUEModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

    FAbsytreeUEStyle::Initialize();
    FAbsytreeUEStyle::ReloadTextures();

    FAbsytreeUECommands::Register();

    if (auto* const SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->RegisterSettings("Project", "Plugins", "Absytree",
                                         LOCTEXT("RuntimeSettingsName", "Absytree"), FText(),
                                         GetMutableDefault<UAbsytreeUESettings>());
    }

    PluginCommands = MakeShareable(new FUICommandList);

    PluginCommands->MapAction(
        FAbsytreeUECommands::Get().OpenPluginWindow,
        FExecuteAction::CreateRaw(this, &FAbsytreeUEModule::PluginButtonClicked),
        FCanExecuteAction());

    PluginCommands->MapAction(
        FAbsytreeUECommands::Get().ReloadDll,
        FExecuteAction::CreateRaw(this, &FAbsytreeUEModule::ReloadDll),
        FCanExecuteAction());

    EditorWrapper = MakeShareable(new FEditorWrapper);

    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAbsytreeUEModule::RegisterMenus));

    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
                                AbsytreeUETabName, FOnSpawnTab::CreateRaw(this, &FAbsytreeUEModule::OnSpawnPluginTab))
                            .SetDisplayName(LOCTEXT("FAbsytreeUETabTitle", "AbsytreeUE"))
                            .SetMenuType(ETabSpawnerMenuType::Hidden);

    ReloadDll();

    // GAbsytreeRunnableThread = FRunnableThread::Create(&GAbsytreeThread, TEXT("Absytree"));

    GAbsytreeUEModule = this;

    FSlateApplication::Get().OnPreTick().AddStatic(HandleSlateTick);
}

void FAbsytreeUEModule::ShutdownModule()
{
    // GAbsytreeRunnableThread->Kill();

    GAbsytreeUEModule = nullptr;

    if (auto* const SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->UnregisterSettings("Project", "Plugins", "Absytree");
    }

    UToolMenus::UnRegisterStartupCallback(this);

    UToolMenus::UnregisterOwner(this);

    FAbsytreeUEStyle::Shutdown();

    FAbsytreeUECommands::Unregister();

    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(AbsytreeUETabName);
}

TSharedRef<SDockTab> FAbsytreeUEModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
    FText WidgetText = FText::Format(
        LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
        FText::FromString(TEXT("FAbsytreeUEModule::OnSpawnPluginTab")),
        FText::FromString(TEXT("AbsytreeUE.cpp"))
    );

    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            // Put your tab content here!
            SNew(SBox)
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            [
                SNew(SAbsytreeView)
                .EditorWrapper(EditorWrapper)
            ]
        ];
}

void FAbsytreeUEModule::PluginButtonClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(AbsytreeUETabName);
}

void FAbsytreeUEModule::ReloadDll() const
{
    const auto* Settings = GetDefault<UAbsytreeUESettings>();

    if (Settings->DllPath.FilePath.IsEmpty())
    {
        UE_LOG(LogAbsytreeUE, Error, TEXT("Missing path for absytree dynamic library"));
        return;
    }

    EditorWrapper->LoadDll(Settings->DllPath);
}

void FAbsytreeUEModule::Tick(float DeltaTime)
{
    const auto* Settings = GetDefault<UAbsytreeUESettings>();

    TimeAccumulator += DeltaTime;
    if (TimeAccumulator >= Settings->PollInterval)
    {
        TimeAccumulator = 0;
        EditorWrapper->Poll(Settings->PollTimeoutMs);
    }
}

void FAbsytreeUEModule::RegisterMenus()
{
    // Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
    FToolMenuOwnerScoped OwnerScoped(this);

    {
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
        FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
        Section.AddMenuEntryWithCommandList(FAbsytreeUECommands::Get().OpenPluginWindow, PluginCommands);
        Section.AddMenuEntryWithCommandList(FAbsytreeUECommands::Get().ReloadDll, PluginCommands);
    }

    {
        UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
        FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
        FToolMenuEntry& Entry = Section.AddEntry(
            FToolMenuEntry::InitToolBarButton(FAbsytreeUECommands::Get().OpenPluginWindow));
        Entry.SetCommandList(PluginCommands);
        FToolMenuEntry& Entry2 = Section.AddEntry(
            FToolMenuEntry::InitToolBarButton(FAbsytreeUECommands::Get().ReloadDll));
        Entry2.SetCommandList(PluginCommands);
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAbsytreeUEModule, AbsytreeUE)
