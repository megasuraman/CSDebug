// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_ScreenWindowTextObject.h
 * @brief デバッグ情報表示用WindowのBlueprint用
 * @author SensyuGames
 * @date 2021/12/27
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebug_ScreenWindowText.h"
#include "CSDebug_ScreenWindowTextObject.generated.h"


DECLARE_DYNAMIC_DELEGATE(FCSDebug_ScreenWindowTextPreDrawDelegate);

/**
 * 
 */
UCLASS(Blueprintable)
class CSDEBUG_API UCSDebug_ScreenWindowTextObject : public UObject
{
    GENERATED_BODY()

    UCSDebug_ScreenWindowTextObject(const FObjectInitializer& ObjectInitializer);
	virtual void BeginDestroy() override;
    
    UFUNCTION(BlueprintCallable)
    void    BeginDraw();
    UFUNCTION(BlueprintCallable)
    void    EndDraw();
        
    UFUNCTION(BlueprintCallable)
    void    SetDrawPos2D(const FVector2D& InPos)
    {
        mDrawPos2D = InPos;
    }

    UFUNCTION(BlueprintCallable)
    void    SetWindowName(const FString& InString)
    {
        mDebugInfoWindow.SetWindowName(InString);
    }

    UFUNCTION(BlueprintCallable)
    void    AddText(const FString& InString)
    {
        mDebugInfoWindow.AddText(InString);
    }

    UFUNCTION(BlueprintCallable)
    void    AddPreDrawDelegate(FCSDebug_ScreenWindowTextPreDrawDelegate InDelegate)
    {
        mPreDrawDelegate = InDelegate;
        mbUsePreDrawDelegate = true;
    }
    
    UFUNCTION(BlueprintCallable)
    void    SetDrawTargetActor(const AActor* InActor)
    {
        mDrawTargetActor = InActor;
    }
    
    UFUNCTION(BlueprintCallable)
    void    SetWindowBackColor(FLinearColor InColor)
    {
        mDebugInfoWindow.SetWindowBackColor(InColor);
    }
    UFUNCTION(BlueprintCallable)
    void    SetWindowFrameColor(FLinearColor InColor)
    {
        mDebugInfoWindow.SetWindowFrameColor(InColor);
    }
    UFUNCTION(BlueprintCallable)
    void    SetFontColor(FLinearColor InColor)
    {
        mDebugInfoWindow.SetFontColor(InColor);
    }

    const FCSDebug_ScreenWindowText& GetDebugInfoWindow() const { return mDebugInfoWindow; }

protected:
    void	DebugDraw(class UCanvas* InCanvas, class APlayerController* InPlayerController);

private:
    FCSDebug_ScreenWindowText    mDebugInfoWindow;
    FCSDebug_ScreenWindowTextPreDrawDelegate mPreDrawDelegate;
    FDelegateHandle	mDebugDrawHandle;
    TWeakObjectPtr<const AActor>  mDrawTargetActor;
    FVector2D   mDrawPos2D = FVector2D::ZeroVector;
    uint8   mbUsePreDrawDelegate : 1;
    uint8 : 7;
};
