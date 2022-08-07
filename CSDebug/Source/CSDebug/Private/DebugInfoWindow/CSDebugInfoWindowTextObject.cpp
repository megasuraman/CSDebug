// Copyright 2020 SensyuGames.
/**
 * @file CSDebugInfoWindowTextObject.cpp
 * @brief デバッグ情報表示用WindowのBlueprint用
 * @author SensyuGames
 * @date 2021/12/27
 */


#include "DebugInfoWindow/CSDebugInfoWindowTextObject.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "CanvasItem.h"
#include "Debug/DebugDrawService.h"


UCSDebugInfoWindowTextObject::UCSDebugInfoWindowTextObject(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,mbUsePreDrawDelegate(false)
{

}

void	UCSDebugInfoWindowTextObject::BeginDestroy()
{
	Super::BeginDestroy();

	EndDraw();
}

/**
 * @brief 表示開始
 */
void	UCSDebugInfoWindowTextObject::BeginDraw()
{
	if (!mDebugDrawHandle.IsValid())
	{
		auto DebugDrawDelegate = FDebugDrawDelegate::CreateUObject(this, &UCSDebugInfoWindowTextObject::DebugDraw);
		if (DebugDrawDelegate.IsBound())
		{
			mDebugDrawHandle = UDebugDrawService::Register(TEXT("GameplayDebug"), DebugDrawDelegate);
		}
	}
}

/**
 * @brief 表示終了
 */
void	UCSDebugInfoWindowTextObject::EndDraw()
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
void	UCSDebugInfoWindowTextObject::DebugDraw(UCanvas* InCanvas, APlayerController* InPlayerController)
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