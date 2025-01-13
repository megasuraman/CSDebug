// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_ScreenWindowText.h
 * @brief デバッグ情報表示用Window　テキスト表示
 * @author SensyuGames
 * @date 2021/12/27
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebug_LoopOrderArray.h"
#include "CSDebug_ScreenWindowBase.h"
#include "CSDebug_ScreenWindowText.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct CSDEBUG_API FCSDebug_ScreenWindowText : public FCSDebug_ScreenWindowBase
{
    GENERATED_USTRUCT_BODY()

    FCSDebug_ScreenWindowText(){}
    virtual ~FCSDebug_ScreenWindowText(){}
 
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
    TCSDebug_LoopOrderArray<FString> mStringList{64};
    FLinearColor	mFontColor = FLinearColor(0.1f, 0.9f, 0.1f, 1.f);
    float   mWidthInterval = 5.f;
    float   mHeightInterval = 2.f;//文字の上下に空ける長さ
    float   mFontWidth = 9.f;
    float   mFontHeight = 15.f;
};