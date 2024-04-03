#pragma once

#include "CoreMinimal.h"
#include "Input/Events.h"
#include "Layout/Geometry.h"
#include "UObject/SoftObjectPath.h"

using AbsytreeApp = struct{};

using DrawRectFn = void(*)(float X, float Y, float Width, float Height, float R, float G, float B, float A);
using DrawTextFn = void(*)(float X, float Y, float R, float G, float B, float A, const char* Text);
using PushClipRectFn = void(*)(float X, float Y, float Width, float Height);
using PopClipRectFn = void(*)();

using OnEditorCreatedFn = void(*)(AbsytreeApp*);
using UnrealLogFn = void(*)(int32, const char*);
using InitializeFn = void(*)(OnEditorCreatedFn, UnrealLogFn);
using DeinitializeFn = void(*)();
using PollFn = void(*)(int32);
using InputFn = void(*)(AbsytreeApp*, const char* Input);
using PaintFn = void(*)(float Width, float Height, DrawRectFn DrawRect, DrawTextFn DrawText, PushClipRectFn PushClipRect, PopClipRectFn PopClipRect);
using KeyEventFn = bool(*)(int64 Input, int32 Modifiers);
using MouseEventFn = bool(*)(int64 Input, int32 Modifiers, float X, float Y);
using MouseMovedEventFn = bool(*)(int64 Input, float X, float Y);

DECLARE_LOG_CATEGORY_EXTERN(LogAbsytreeUE, Log, Log);

class FEditorWrapper
{
public:
    void Poll(int32 TimeoutMs);

    void HandlePaintAbsytree(float Width, float Height, DrawRectFn InDrawRect, DrawTextFn InDrawText, PushClipRectFn InPushClipRect, PopClipRectFn InPopClipRect) const;
    bool HandleMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const;
    bool HandleMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const;
    bool HandleMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const;
    bool HandleMouseDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const;
    bool HandleMouseMoved(const FGeometry& Geometry, const FPointerEvent& MouseEvent) const;
    bool HandleKeyChar(const FCharacterEvent& Event) const;
    bool HandleKeyDown(const FKeyEvent& Event) const;
    bool HandleKeyUp(const FKeyEvent& Event) const;

    void LoadDll(const FFilePath& Path);

private:
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

    Windows::HMODULE DllHandle = nullptr;

    int32 DllVersion = 0;
};
