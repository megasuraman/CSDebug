// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_ScreenWindowTextObject.cpp
 * @brief デバッグ情報表示用WindowのBlueprint用
 * @author SensyuGames
 * @date 2021/12/27
 */


#include "ScreenWindow/CSDebug_ScreenWindowTextObject.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "CanvasItem.h"
#include "Debug/DebugDrawService.h"


UCSDebug_ScreenWindowTextObject::UCSDebug_ScreenWindowTextObject(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,mbUsePreDrawDelegate(false)
{

}

void	UCSDebug_ScreenWindowTextObject::BeginDestroy()
{
	Super::BeginDestroy();

	EndDraw();
}

/**
 * @brief 表示開始
 */
void	UCSDebug_ScreenWindowTextObject::BeginDraw()
{
	if (!mDebugDrawHandle.IsValid())
	{
		auto DebugDrawDelegate = FDebugDrawDelegate::CreateUObject(this, &UCSDebug_ScreenWindowTextObject::DebugDraw);
		if (DebugDrawDelegate.IsBound())
		{
			mDebugDrawHandle = UDebugDrawService::Register(TEXT("GameplayDebug"), DebugDrawDelegate);
		}
	}
}

/**
 * @brief 表示終了
 */
void	UCSDebug_ScreenWindowTextObject::EndDraw()
{
	if (mDebugDrawHandle.IsValid())
	{
		UDebugDrawService::Unregister(mDebugDrawHandle);
		mDebugDrawHandle.Reset();
	}
}

/**
 * @brief 表示
 */
void	UCSDebug_ScreenWindowTextObject::DebugDraw(UCanvas* InCanvas, APlayerController* InPlayerController)
{
	if (mbUsePreDrawDelegate)
	{
		mPreDrawDelegate.Execute();
	}

	const AActor* TargetActor = mDrawTargetActor.Get();
	if (TargetActor)
	{
		mDebugInfoWindow.Draw(InCanvas, TargetActor->GetActorLocation());
	}
	else
	{
		mDebugInfoWindow.Draw(InCanvas, mDrawPos2D);
	}

	mDebugInfoWindow.ClearString();
}