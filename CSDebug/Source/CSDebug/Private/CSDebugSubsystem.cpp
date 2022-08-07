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
#include "CSDebugConfig.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "CanvasItem.h"
#include "Debug/DebugDrawService.h"

/**
 * @brief Initialize
 */
void	UCSDebugSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
#if USE_CSDEBUG
	RequestTick(true);
	RequestDraw(true);

	UWorld* Wold = GetWorld();

	if (mDebugMenuManager == nullptr)
	{
		mDebugMenuManager = NewObject<UCSDebugMenuManager>(this);
		mDebugMenuManager->Init();
	}
	if (mDebugCommand == nullptr)
	{
		mDebugCommand = NewObject<UCSDebugCommand>(this);
		mDebugCommand->Init();
	}
	if (mDebugSelectManager == nullptr)
	{
		mDebugSelectManager = NewObject<UCSDebugSelectManager>(this);
		mDebugSelectManager->Init();
	}
#endif //USE_CSDEBUG
}
/**
 * @brief Deinitialize
 */
void	UCSDebugSubsystem::Deinitialize()
{
#if USE_CSDEBUG
	RequestTick(false);
	RequestDraw(false);
#endif //USE_CSDEBUG
}

/**
 * @brief	Tick“o˜^‚Ìon/off
 */
void	UCSDebugSubsystem::RequestTick(const bool bInActive)
{
#if USE_CSDEBUG
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
#endif //USE_CSDEBUG
}

/**
 * @brief	Draw“o˜^‚Ìon/off
 */
void	UCSDebugSubsystem::RequestDraw(const bool bInActive)
{
#if USE_CSDEBUG
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
#endif //USE_CSDEBUG
}

/**
 * @brief	Tick
 */
bool	UCSDebugSubsystem::DebugTick(float InDeltaSecond)
{
#if USE_CSDEBUG
	const UCSDebugConfig* CSDebugConfig = GetDefault<UCSDebugConfig>();
	if (!CSDebugConfig->mbActiveCSDebug)
	{
		return true;
	}

	if (mDebugCommand)
	{
		mDebugCommand->DebugTick(InDeltaSecond);
	}
	if (mDebugMenuManager)
	{
		mDebugMenuManager->DebugTick(InDeltaSecond);
	}
	if (mDebugSelectManager)
	{
		mDebugSelectManager->DebugTick(InDeltaSecond);
	}
#endif //USE_CSDEBUG

	return true;
}
/**
 * @brief	Draw
 */
void	UCSDebugSubsystem::DebugDraw(UCanvas* InCanvas, APlayerController* InPlayerController)
{
#if USE_CSDEBUG
	if (mDebugCommand)
	{
		mDebugCommand->DebugDraw(InCanvas);
	}
	if (mDebugMenuManager)
	{
		mDebugMenuManager->DebugDraw(InCanvas);
	}
	if (mDebugSelectManager)
	{
		mDebugSelectManager->DebugDraw(InCanvas);
	}
#endif //USE_CSDEBUG
}