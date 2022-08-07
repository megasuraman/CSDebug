// Copyright 2020 SensyuGames.
/**
 * @file CSDebugInfoWindowText.h
 * @brief デバッグ情報表示用Window　テキスト表示
 * @author SensyuGames
 * @date 2021/12/27
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebugLoopOrderArray.h"
#include "CSDebugInfoWindowBase.h"
#include "CSDebugInfoWindowText.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct CSDEBUG_API FCSDebugInfoWindowText : public FCSDebugInfoWindowBase
{
    GENERATED_USTRUCT_BODY()

    FCSDebugInfoWindowText(){}
    virtual ~FCSDebugInfoWindowText(){}
 
public:
	virtual void    FittingWindowExtent(class UCanvas* InCanvas) override;

    void    AddText(const FString& InString);
    void    ClearString()
    {
        mStringList.Clear();
        SetWindowExtent(FVector2D::ZeroVector);
    }
    void    SetFontColor(const FLinearColor& InColor) { mFontColor = InColor; }

protected:
    virtual void    DrawAfterBackground(class UCanvas* InCanvas, const FVector2D& InPos2D) const override;
 
private:
    TCSDebugLoopOrderArray<FString> mStringList{64};
    FLinearColor	mFontColor = FLinearColor(0.1f, 0.9f, 0.1f, 1.f);
    float   mWidthInterval = 5.f;
    float   mHeightInterval = 2.f;//文字の上下に空ける長さ
    float   mFontWidth = 9.f;
    float   mFontHeight = 15.f;
};