// Copyright 2020 SensyuGames.
/**
 * @file CSDebugSubsystem.cpp
 * @brief CSDebug—p‚ÌSubsystem
 * @author SensyuGames
 * @date 2020/7/24
 */


#include "CSDebugSubsystem.h"

#include "CSDebugCommand.h"
#include "DebugSelect/CSDebugSelectManager.h"
#include "DebugMenu/CSDebugMenuManager.h"
#include "DebugInfoWindow/CSDebugInfoWindowManager.h"
#include "CSDebugConfig.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "CanvasItem.h"
#include "Debug/DebugDrawService.h"

 /**
  * @brief BP—p‚ÌGetter
  */
UCSDebugMenuManager* UCSDebugSubsystem::GetDebugMenuManagerBP() const
{
#if USE_CSDEBUG
	return mGCObject.mDebugMenuManager;
#else
	return nullptr;
#endif
}
UCSDebugInfoWindowManager* UCSDebugSubsystem::GetDebugInfoWindowManagerBP() const
{
#if USE_CSDEBUG
	return mGCObject.mDebugInfoWindowManager;
#else
	return nullptr;
#endif
}

#if USE_CSDEBUG
/**
 * @brief Initialize
 */
void	UCSDebugSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	RequestTick(true);
	RequestDraw(true);

	UWorld* Wold = GetWorld();

	if (mGCObject.mDebugMenuManager == nullptr)
	{
		mGCObject.mDebugMenuManager = NewObject<UCSDebugMenuManager>(this);
		mGCObject.mDebugMenuManager->Init();
	}
	if (mGCObject.mDebugCommand == nullptr)
	{
		mGCObject.mDebugCommand = NewObject<UCSDebugCommand>(this);
		mGCObject.mDebugCommand->Init();
	}
	if (mGCObject.mDebugSelectManager == nullptr)
	{
		mGCObject.mDebugSelectManager = NewObject<UCSDebugSelectManager>(this);
		mGCObject.mDebugSelectManager->Init();
	}
	if (mGCObject.mDebugInfoWindowManager == nullptr)
	{
		mGCObject.mDebugInfoWindowManager = NewObject<UCSDebugInfoWindowManager>(this);
		mGCObject.mDebugInfoWindowManager->Init();
	}
}
/**
 * @brief Deinitialize
 */
void	UCSDebugSubsystem::Deinitialize()
{
	RequestTick(false);
	RequestDraw(false);
}

/**
 * @brief	Tick“o˜^‚Ìon/off
 */
void	UCSDebugSubsystem::RequestTick(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugTickHandle.IsValid())
		{
			mDebugTickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UCSDebugSubsystem::DebugTick));
		}
	}
	else
	{
		FTicker::GetCoreTicker().RemoveTicker(mDebugTickHandle);
	}
}

/**
 * @brief	Draw“o˜^‚Ìon/off
 */
void	UCSDebugSubsystem::RequestDraw(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugDrawHandle.IsValid())
		{
			auto DebugDrawDelegate = FDebugDrawDelegate::CreateUObject(this, &UCSDebugSubsystem::DebugDraw);
			if (DebugDrawDelegate.IsBound())
			{
				mDebugDrawHandle = UDebugDrawService::Register(TEXT("GameplayDebug"), DebugDrawDelegate);
			}
		}
	}
	else
	{
		if (mDebugDrawHandle.IsValid())
		{
			UDebugDrawService::Unregister(mDebugDrawHandle);
			mDebugDrawHandle.Reset();
		}
	}
}

/**
 * @brief	Tick
 */
bool	UCSDebugSubsystem::DebugTick(float InDeltaSecond)
{
	const UCSDebugConfig* CSDebugConfig = GetDefault<UCSDebugConfig>();
	if (!CSDebugConfig->mbActiveCSDebug)
	{
		return true;
	}

	if (mGCObject.mDebugCommand)
	{
		mGCObject.mDebugCommand->DebugTick(InDeltaSecond);
	}
	if (mGCObject.mDebugMenuManager)
	{
		mGCObject.mDebugMenuManager->DebugTick(InDeltaSecond);
	}
	if (mGCObject.mDebugSelectManager)
	{
		mGCObject.mDebugSelectManager->DebugTick(InDeltaSecond);
	}
	if (mGCObject.mDebugInfoWindowManager)
	{
		mGCObject.mDebugInfoWindowManager->DebugTick(InDeltaSecond);
	}

	return true;
}
/**
 * @brief	Draw
 */
void	UCSDebugSubsystem::DebugDraw(UCanvas* InCanvas, APlayerController* InPlayerController)
{
	if (mGCObject.mDebugCommand)
	{
		mGCObject.mDebugCommand->DebugDraw(InCanvas);
	}
	if (mGCObject.mDebugMenuManager)
	{
		mGCObject.mDebugMenuManager->DebugDraw(InCanvas);
	}
	if (mGCObject.mDebugSelectManager)
	{
		mGCObject.mDebugSelectManager->DebugDraw(InCanvas);
	}
	if (mGCObject.mDebugInfoWindowManager)
	{
		mGCObject.mDebugInfoWindowManager->DebugDraw(InCanvas);
	}
}
#endif