#include "EditorWrapper.h"

#include "Windows/MinimalWindowsApi.h"
#include <windows.h>
#include <filesystem>

namespace fs = std::filesystem;

constexpr int64 ABSYTREE_INPUT_ENTER = -1;
constexpr int64 ABSYTREE_INPUT_ESCAPE = -2;
constexpr int64 ABSYTREE_INPUT_BACKSPACE = -3;
constexpr int64 ABSYTREE_INPUT_SPACE = -4;
constexpr int64 ABSYTREE_INPUT_DELETE = -5;
constexpr int64 ABSYTREE_INPUT_TAB = -6;
constexpr int64 ABSYTREE_INPUT_LEFT = -7;
constexpr int64 ABSYTREE_INPUT_RIGHT = -8;
constexpr int64 ABSYTREE_INPUT_UP = -9;
constexpr int64 ABSYTREE_INPUT_DOWN = -10;
constexpr int64 ABSYTREE_INPUT_HOME = -11;
constexpr int64 ABSYTREE_INPUT_END = -12;
constexpr int64 ABSYTREE_INPUT_PAGE_UP = -13;
constexpr int64 ABSYTREE_INPUT_PAGE_DOWN = -14;
constexpr int64 ABSYTREE_INPUT_F1 = -20;
constexpr int64 ABSYTREE_INPUT_F2 = -21;
constexpr int64 ABSYTREE_INPUT_F3 = -22;
constexpr int64 ABSYTREE_INPUT_F4 = -23;
constexpr int64 ABSYTREE_INPUT_F5 = -24;
constexpr int64 ABSYTREE_INPUT_F6 = -25;
constexpr int64 ABSYTREE_INPUT_F7 = -26;
constexpr int64 ABSYTREE_INPUT_F8 = -27;
constexpr int64 ABSYTREE_INPUT_F9 = -28;
constexpr int64 ABSYTREE_INPUT_F10 = -29;
constexpr int64 ABSYTREE_INPUT_F11 = -30;
constexpr int64 ABSYTREE_INPUT_F12 = -31;

extern "C" void HandleAppCreatedC(AbsytreeApp* App)
{
	UE_LOG(LogAbsytreeUE, Log, TEXT("Absytree App created"));
}

extern "C" void UnrealLog(int32 Level, const char* message)
{
	switch (Level)
	{
	case 6: UE_LOG(LogAbsytreeUE, Fatal, TEXT("%hs"), message);
		break;
	case 5: UE_LOG(LogAbsytreeUE, Error, TEXT("%hs"), message);
		break;
	case 4: UE_LOG(LogAbsytreeUE, Warning, TEXT("%hs"), message);
		break;
	case 2: UE_LOG(LogAbsytreeUE, Log, TEXT("%hs"), message);
		break;
	case 1: UE_LOG(LogAbsytreeUE, Display, TEXT("%hs"), message);
		break;
	case 0: UE_LOG(LogAbsytreeUE, VeryVerbose, TEXT("%hs"), message);
		break;
	default: break;
	}
}

int32 GetMods(const FInputEvent& Event)
{
	int32 Mods = 0;
	if (Event.IsControlDown())
	{
		Mods |= 0x1;
	}
	if (Event.IsShiftDown())
	{
		Mods |= 0x2;
	}
	if (Event.IsAltDown())
	{
		Mods |= 0x4;
	}
	if (Event.IsCommandDown())
	{
		Mods |= 0x8;
	}
	return Mods;
}

int64 GetInputFromButton(const FKey& Key)
{
	if (Key.GetFName() == "LeftMouseButton")
	{
		return 0;
	}
	if (Key.GetFName() == "RightMouseButton")
	{
		return 2;
	}

	return INDEX_NONE;
}

void GetButtonAndMods(const FPointerEvent& MouseEvent, int64& OutButton, int32& OutMods)
{
	OutButton = GetInputFromButton(MouseEvent.GetEffectingButton());
	if (OutButton == INDEX_NONE && MouseEvent.GetPressedButtons().Num() > 0)
	{
		OutButton = GetInputFromButton(*MouseEvent.GetPressedButtons().begin());
	}

	OutMods = GetMods(MouseEvent);
}

int64 GetInput(int32 Character)
{
	switch (Character)
	{
	case 8: return ABSYTREE_INPUT_BACKSPACE;
	case 9: return ABSYTREE_INPUT_TAB;
	case 13: return ABSYTREE_INPUT_ENTER;
	case 27: return ABSYTREE_INPUT_ESCAPE;
	case 32: return ABSYTREE_INPUT_SPACE;
	default:
		return Character;
	}
}

void GetKeyAndMods(const FKeyEvent& Event, int64& OutInput, int32& OutMods)
{
	OutMods = GetMods(Event);

	OutInput = 0;
	switch (Event.GetKeyCode())
	{
	case 33: OutInput = ABSYTREE_INPUT_PAGE_UP;
		break;
	case 34: OutInput = ABSYTREE_INPUT_PAGE_DOWN;
		break;
	case 35: OutInput = ABSYTREE_INPUT_END;
		break;
	case 36: OutInput = ABSYTREE_INPUT_HOME;
		break;
	case 37: OutInput = ABSYTREE_INPUT_LEFT;
		break;
	case 38: OutInput = ABSYTREE_INPUT_UP;
		break;
	case 39: OutInput = ABSYTREE_INPUT_RIGHT;
		break;
	case 40: OutInput = ABSYTREE_INPUT_DOWN;
		break;
	case 46: OutInput = ABSYTREE_INPUT_DELETE;
		break;
	default:
		break;
	}

	if (Event.GetCharacter() != 0 && (Event.IsAltDown() || Event.IsCommandDown() || Event.IsControlDown()))
	{
		const auto Char = Event.GetCharacter();
		if (!Event.IsShiftDown())
		{
			OutInput = static_cast<int64>(reinterpret_cast<uintptr_t>(CharLowerW(reinterpret_cast<LPWSTR>(Char))));
		}
		else
		{
			OutInput = Char;
		}
	}
}

void GetKeyAndMods(const FCharacterEvent& Event, int64& OutInput, int32& OutMods)
{
	OutMods = GetMods(Event);
	if (Event.IsAltDown() || Event.IsCommandDown() || Event.IsControlDown())
	{
		OutInput = 0;
	}
	else
	{
		auto Char = Event.GetCharacter();
		if (!Event.IsShiftDown())
		{
			Char = static_cast<TCHAR>(reinterpret_cast<uintptr_t>(CharLowerW(reinterpret_cast<LPWSTR>(Char))));
		}
		OutInput = GetInput(Char);
	}
}

bool FEditorWrapper::HandleMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const
{
	if (MouseScroll)
	{
		const auto Pos = MouseEvent.GetScreenSpacePosition() - MyGeometry.AbsolutePosition;
		int64 Input;
		int32 Mods;
		GetButtonAndMods(MouseEvent, Input, Mods);
		return MouseScroll(MouseEvent.GetWheelDelta(), Mods, Pos.X, Pos.Y);
	}
	return false;
}

bool FEditorWrapper::HandleMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const
{
	if (MouseDown)
	{
		const auto Pos = MouseEvent.GetScreenSpacePosition() - MyGeometry.AbsolutePosition;
		int64 Input;
		int32 Mods;
		GetButtonAndMods(MouseEvent, Input, Mods);
		return MouseDown(Input, Mods, Pos.X, Pos.Y);
	}
	return false;
}

bool FEditorWrapper::HandleMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const
{
	if (MouseUp)
	{
		const auto Pos = MouseEvent.GetScreenSpacePosition() - MyGeometry.AbsolutePosition;
		int64 Input;
		int32 Mods;
		GetButtonAndMods(MouseEvent, Input, Mods);
		return MouseUp(Input, Mods, Pos.X, Pos.Y);
	}
	return false;
}

bool FEditorWrapper::HandleMouseDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const
{
	if (MouseDown)
	{
		const auto Pos = MouseEvent.GetScreenSpacePosition() - MyGeometry.AbsolutePosition;
		int64 Input;
		int32 Mods;
		GetButtonAndMods(MouseEvent, Input, Mods);
		return MouseDown(3, Mods, Pos.X, Pos.Y);
	}
	return false;
}

bool FEditorWrapper::HandleMouseMoved(const FGeometry& Geometry, const FPointerEvent& MouseEvent) const
{
	if (MouseMoved)
	{
		const auto Pos = MouseEvent.GetScreenSpacePosition() - Geometry.AbsolutePosition;
		int64 Input;
		int32 Mods;
		GetButtonAndMods(MouseEvent, Input, Mods);
		return MouseMoved(Input, Pos.X, Pos.Y);
	}
	return false;
}

bool FEditorWrapper::HandleKeyChar(const FCharacterEvent& Event) const
{
	int64 Input = 0;
	int32 Mods = 0;
	GetKeyAndMods(Event, Input, Mods);
	if (KeyDown && Input != 0)
	{
		return KeyDown(Input, Mods);
	}
	return false;
}

bool FEditorWrapper::HandleKeyDown(const FKeyEvent& Event) const
{
	int64 Input = 0;
	int32 Mods = 0;
	GetKeyAndMods(Event, Input, Mods);
	if (KeyDown && Input != 0)
	{
		return KeyDown(Input, Mods);
	}
	return false;
}

bool FEditorWrapper::HandleKeyUp(const FKeyEvent& Event) const
{
	int64 Input = 0;
	int32 Mods = 0;
	GetKeyAndMods(Event, Input, Mods);
	if (KeyUp && Input != 0)
	{
		return KeyUp(Input, Mods);
	}
	return false;
}

void FEditorWrapper::LoadDll(const FFilePath& Path)
{
	if (DllHandle)
	{
		DeinitializeAbsytree();
		if (!Windows::FreeLibrary(DllHandle))
		{
			UE_LOG(LogAbsytreeUE, Error, TEXT("Failed to free library"));
		}
	}

	DllHandle = nullptr;

	InitializeAbsytree = nullptr;
	DeinitializeAbsytree = nullptr;
	PollAbsytree = nullptr;
	InputAbsytree = nullptr;
	PaintAbsytree = nullptr;
	KeyDown = nullptr;
	KeyUp = nullptr;
	MouseDown = nullptr;
	MouseUp = nullptr;
	MouseScroll = nullptr;
	MouseMoved = nullptr;

	FString DllPath;
	FString DllName;
	FString DllExtension;
	FPaths::Split(Path.FilePath, OUT DllPath, OUT DllName, OUT DllExtension);

	const auto TempPath = FString::Printf(TEXT("%s/%s-%d.%s"), *DllPath, *DllName, DllVersion, *DllExtension);
	DllVersion++;

	const auto OriginalPath = FString::Printf(TEXT("%s/%s.%s"), *DllPath, *DllName, *DllExtension);
	fs::copy_file(*OriginalPath, *TempPath, fs::copy_options::overwrite_existing);

	DllHandle = Windows::LoadLibraryW(*TempPath);
	if (!DllHandle)
	{
		UE_LOG(LogAbsytreeUE, Error, TEXT("Failed to load absytree dll."));
		return;
	}

#define FIND_ABSYTREE_FUNCTION(var, type, name) var = reinterpret_cast<type>(GetProcAddress(DllHandle, name));\
	if (!var) {\
		UE_LOG(LogAbsytreeUE, Error, TEXT("Failed to find function %s."), TEXT(name));\
		return;\
	}

	FIND_ABSYTREE_FUNCTION(InitializeAbsytree, InitializeFn, "absytree_init");
	FIND_ABSYTREE_FUNCTION(DeinitializeAbsytree, DeinitializeFn, "absytree_shutdown");
	FIND_ABSYTREE_FUNCTION(PollAbsytree, PollFn, "absytree_poll");
	FIND_ABSYTREE_FUNCTION(InputAbsytree, InputFn, "absytree_input_keys");
	FIND_ABSYTREE_FUNCTION(PaintAbsytree, PaintFn, "absytree_render");
	FIND_ABSYTREE_FUNCTION(KeyDown, KeyEventFn, "absytree_key_down");
	FIND_ABSYTREE_FUNCTION(KeyUp, KeyEventFn, "absytree_key_up");
	FIND_ABSYTREE_FUNCTION(MouseDown, MouseEventFn, "absytree_mouse_down");
	FIND_ABSYTREE_FUNCTION(MouseUp, MouseEventFn, "absytree_mouse_up");
	FIND_ABSYTREE_FUNCTION(MouseScroll, MouseEventFn, "absytree_mouse_scroll");
	FIND_ABSYTREE_FUNCTION(MouseMoved, MouseMovedEventFn, "absytree_mouse_moved");

	InitializeAbsytree(HandleAppCreatedC, UnrealLog);
}

void FEditorWrapper::Poll(int32 TimeoutMs)
{
	if (PollAbsytree)
	{
		PollAbsytree(TimeoutMs);
	}
}

void FEditorWrapper::HandlePaintAbsytree(float Width, float Height, DrawRectFn InDrawRect, DrawTextFn InDrawText, PushClipRectFn InPushClipRect, PopClipRectFn InPopClipRect) const
{
	if (PaintAbsytree)
	{
		PaintAbsytree(Width, Height, InDrawRect, InDrawText, InPushClipRect, InPopClipRect);
	}
}
