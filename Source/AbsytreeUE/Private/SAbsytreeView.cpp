#include "SAbsytreeView.h"

#include "EditorWrapper.h"
#include "HAL/Platform.h"
#include "Styling/CoreStyle.h"
#include "Brushes/SlateColorBrush.h"
#include "Framework/Application/SlateApplication.h"

struct FPaintContext
{
    const FGeometry* AllottedGeometry = nullptr;
    const FSlateRect* MyCullingRect = nullptr;
    FSlateWindowElementList* OutDrawElements = nullptr;
    int32 LayerId = 0;
    const FSlateFontInfo* Font = nullptr;
    const FSlateBrush* Brush = nullptr;
};

FPaintContext GPaintContext;

extern "C" void UnrealDrawRect(float X, float Y, float Width, float Height, float R, float G, float B, float A)
{
    if (!GPaintContext.OutDrawElements)
    {
        // UE_LOG(LogTemp, Error, TEXT("DrawRect %f, %f, %f, %f - Not in valid paint context"), X, Y, Width, Height);
        return;
    }

    FSlateDrawElement::MakeBox(
        *GPaintContext.OutDrawElements,
        GPaintContext.LayerId,
        GPaintContext.AllottedGeometry->ToPaintGeometry(FVector2f(Width, Height), FSlateLayoutTransform(FVector2f(X, Y))),
        GPaintContext.Brush,
        ESlateDrawEffect::None,
        FColor(R * 255, G * 255, B * 255, A * 255));
}

extern "C" void UnrealDrawText(float X, float Y, float R, float G, float B, float A, const UTF8CHAR* Text)
{
    if (!GPaintContext.OutDrawElements)
    {
        // UE_LOG(LogTemp, Error, TEXT("DrawRect %f, %f, %f, %f - Not in valid paint context"), X, Y, Width, Height);
        return;
    }

    FSlateDrawElement::MakeText(
        *GPaintContext.OutDrawElements,
        GPaintContext.LayerId,
        GPaintContext.AllottedGeometry->ToPaintGeometry(FSlateLayoutTransform(FVector2D(X, Y))),
        FString(Text),
        *GPaintContext.Font,
        ESlateDrawEffect::None,
        FColor(R * 255, G * 255, B * 255, A * 255)
    );
}

extern "C" void UnrealPushClipRect(float X, float Y, float Width, float Height)
{
    if (!GPaintContext.OutDrawElements)
    {
        return;
    }
    const auto Position = GPaintContext.AllottedGeometry->AbsolutePosition + FVector2f(X, Y);
    GPaintContext.OutDrawElements->PushClip(FSlateClippingZone(FSlateRect(Position.X, Position.Y, Position.X + Width, Position.Y + Height)));
}

extern "C" void UnrealPopClipRect()
{
    if (!GPaintContext.OutDrawElements)
    {
        return;
    }

    GPaintContext.OutDrawElements->PopClip();
}

void SAbsytreeView::Construct(const FArguments& InArgs)
{
    EditorWrapper = InArgs._EditorWrapper;
    ChildSlot
    [
        SNew(SOverlay)
        .Visibility(EVisibility::SelfHitTestInvisible)
    ];
}

int32 SAbsytreeView::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
                             FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
                             bool bParentEnabled) const
{
    const FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle("Mono", 12);
    // const FSlateBrush* Brush = FAbsytreeUEStyle::Get().GetBrush("WhiteBrush");
    const FSlateColorBrush Brush(FLinearColor::White);


    GPaintContext.AllottedGeometry = &AllottedGeometry;
    GPaintContext.MyCullingRect = &MyCullingRect;
    GPaintContext.OutDrawElements = &OutDrawElements;
    GPaintContext.LayerId = LayerId;
    GPaintContext.Font = &Font;
    GPaintContext.Brush = &Brush;

    if (EditorWrapper)
    {
        EditorWrapper->HandlePaintAbsytree(AllottedGeometry.Size.X, AllottedGeometry.Size.Y, UnrealDrawRect, UnrealDrawText, UnrealPushClipRect, UnrealPopClipRect);
    }

    GPaintContext.AllottedGeometry = nullptr;
    GPaintContext.MyCullingRect = nullptr;
    GPaintContext.OutDrawElements = nullptr;
    GPaintContext.LayerId = 0;
    GPaintContext.Font = nullptr;
    GPaintContext.Brush = nullptr;

    return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
                                    bParentEnabled);
}

FVector2D SAbsytreeView::ComputeDesiredSize(float X) const
{
    return SCompoundWidget::ComputeDesiredSize(X);
}

void SAbsytreeView::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

FReply SAbsytreeView::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (EditorWrapper && EditorWrapper->HandleMouseWheel(MyGeometry,  MouseEvent))
        return FReply::Handled();
    return SCompoundWidget::OnMouseWheel(MyGeometry, MouseEvent);
}

FReply SAbsytreeView::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    FSlateApplication::Get().SetKeyboardFocus(AsShared(), EFocusCause::Mouse);
    if (EditorWrapper && EditorWrapper->HandleMouseButtonDown(MyGeometry,  MouseEvent))
        return FReply::Handled();

    return SCompoundWidget::OnMouseButtonDown(MyGeometry, MouseEvent);
}

FReply SAbsytreeView::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (EditorWrapper && EditorWrapper->HandleMouseButtonUp(MyGeometry,  MouseEvent))
        return FReply::Handled();
    return SCompoundWidget::OnMouseButtonUp(MyGeometry, MouseEvent);
}

FReply SAbsytreeView::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (EditorWrapper && EditorWrapper->HandleMouseDoubleClick(MyGeometry,  MouseEvent))
        return FReply::Handled();
    return SCompoundWidget::OnMouseButtonDoubleClick(MyGeometry, MouseEvent);
}

FReply SAbsytreeView::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (EditorWrapper && EditorWrapper->HandleMouseMoved(MyGeometry,  MouseEvent))
        return FReply::Handled();
    return SCompoundWidget::OnMouseMove(MyGeometry, MouseEvent);
}

FReply SAbsytreeView::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent)
{
    if (EditorWrapper && EditorWrapper->HandleKeyChar(InCharacterEvent))
        return FReply::Handled();
    return SCompoundWidget::OnKeyChar(MyGeometry, InCharacterEvent);
}

FReply SAbsytreeView::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
    if (EditorWrapper && EditorWrapper->HandleKeyDown(InKeyEvent))
        return FReply::Handled();
    return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

FReply SAbsytreeView::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
    if (EditorWrapper && EditorWrapper->HandleKeyUp(InKeyEvent))
        return FReply::Handled();
    return SCompoundWidget::OnKeyUp(MyGeometry, InKeyEvent);
}
