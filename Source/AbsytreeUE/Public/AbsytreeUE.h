// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class FAbsytreeUEModule;
// class AbsytreeApp;

using AbsytreeApp = int32;

using OnEditorCreatedFn = void(*)(AbsytreeApp*, FAbsytreeUEModule*);
using UnrealLogFn = void(*)(int32, const char*);
using InitializeFn = void(*)(FAbsytreeUEModule*, OnEditorCreatedFn, UnrealLogFn);
using DeinitializeFn = void(*)();
using PollFn = void(*)(int32);
using InputFn = void(*)(AbsytreeApp*, const char* Input);
using PaintFn = void(*)(float Width, float Height);
using KeyEventFn = bool(*)(int64 Input, int32 Modifiers);
using MouseEventFn = bool(*)(int64 Input, int32 Modifiers, float X, float Y);
using MouseMovedEventFn = bool(*)(int64 Input, float X, float Y);

extern PollFn GPollAbsytree;
extern FAbsytreeUEModule* GAbsytreeUEModule;

class FAbsytreeUEModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();

	/** This function will be bound to Command (by default it will bring up plugin window) */
	void ReloadDll();

	void HandleAppCreated(AbsytreeApp* App);

	void Tick(float DeltaTime);

	void HandlePaintAbsytree(float Width, float Height);

	bool HandleMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	bool HandleMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	bool HandleMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	bool HandleMouseDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	bool HandleMouseMoved(const FGeometry& Geometry, const FPointerEvent& MouseEvent);

	bool HandleKeyChar(const FCharacterEvent& Event);
	bool HandleKeyDown(const FKeyEvent& Event);
	bool HandleKeyUp(const FKeyEvent& Event);

private:
	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

	float TimeAccumulator = 0;
	int32 DllVersion = 0;

	InitializeFn InitializeAbsytree = nullptr;
	DeinitializeFn DeinitializeAbsytree = nullptr;
	PollFn PollAbsytree = nullptr;
	InputFn InputAbsytree = nullptr;
	PaintFn PaintAbsytree = nullptr;
	KeyEventFn KeyDown = nullptr;
	KeyEventFn KeyUp = nullptr;
	MouseEventFn MouseDown = nullptr;
	MouseEventFn MouseUp = nullptr;
	MouseEventFn MouseScroll = nullptr;
	MouseMovedEventFn MouseMoved = nullptr;

	FString DllName = "ast";
	FString DllPath = "D:/Absytree";

	TSharedPtr<class FUICommandList> PluginCommands;

	Windows::HMODULE DllHandle = nullptr;
};
