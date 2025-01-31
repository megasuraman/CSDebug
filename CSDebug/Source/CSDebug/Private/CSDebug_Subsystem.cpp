// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_Subsystem.cpp
 * @brief CSDebug_Subsystem
 * @author SensyuGames
 * @date 2020/7/24
 */


#include "CSDebug_Subsystem.h"

#include "CSDebug_ShortcutCommand.h"
#include "ActorSelect/CSDebug_ActorSelectManager.h"
#include "DebugMenu/CSDebug_DebugMenuManager.h"
#include "ScreenWindow/CSDebug_ScreenWindowManager.h"
#include "CSDebug_Config.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "CanvasItem.h"
#include "Debug/DebugDrawService.h"

DEFINE_LOG_CATEGORY(CSDebugLog);

FCSDebug_SaveData UCSDebug_Subsystem::mSaveData;

FCSDebug_SaveData& UCSDebug_Subsystem::sGetSaveData()
{
	mSaveData.Load();
	return mSaveData;
}

/**
 * @brief 
 */
UCSDebug_ScreenWindowManager* UCSDebug_Subsystem::GetScreenWindowManagerBP() const
{
#if USE_CSDEBUG
	return mGCObject.mScreenWindowManager;
#else
	return nullptr;
#endif
}

#if USE_CSDEBUG

/**
 * @brief Initialize
 */
void	UCSDebug_Subsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	RequestTick(true);
	RequestDraw(true);

	UWorld* Wold = GetWorld();

	if (mGCObject.mDebugMenuManager == nullptr)
	{
		const UCSDebug_Config* CSDebugConfig = GetDefault<UCSDebug_Config>();
		mGCObject.mDebugMenuManager = NewObject<UCSDebug_DebugMenuManager>(this, CSDebugConfig->mDebugMenuManagerClass);
		mGCObject.mDebugMenuManager->Init();
	}
	if (mGCObject.mShortcutCommand == nullptr)
	{
		mGCObject.mShortcutCommand = NewObject<UCSDebug_ShortcutCommand>(this);
		mGCObject.mShortcutCommand->Init();
	}
	if (mGCObject.mActorSelectManager == nullptr)
	{
		mGCObject.mActorSelectManager = NewObject<UCSDebug_ActorSelectManager>(this);
		mGCObject.mActorSelectManager->Init();
	}
	if (mGCObject.mScreenWindowManager == nullptr)
	{
		mGCObject.mScreenWindowManager = NewObject<UCSDebug_ScreenWindowManager>(this);
		mGCObject.mScreenWindowManager->Init();
	}
}
/**
 * @brief Deinitialize
 */
void	UCSDebug_Subsystem::Deinitialize()
{
	RequestTick(false);
	RequestDraw(false);
}

/**
 * @brief	Tickのon/off
 */
void	UCSDebug_Subsystem::RequestTick(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugTickHandle.IsValid())
		{
			mDebugTickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UCSDebug_Subsystem::DebugTick));
		}
	}
	else
	{
		FTicker::GetCoreTicker().RemoveTicker(mDebugTickHandle);
	}
}

/**
 * @brief	Drawのon/off
 */
void	UCSDebug_Subsystem::RequestDraw(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugDrawHandle.IsValid())
		{
			auto DebugDrawDelegate = FDebugDrawDelegate::CreateUObject(this, &UCSDebug_Subsystem::DebugDraw);
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
bool	UCSDebug_Subsystem::DebugTick(float InDeltaSecond)
{
	const UCSDebug_Config* CSDebugConfig = GetDefault<UCSDebug_Config>();
	if (!CSDebugConfig->mbActiveCSDebug)
	{
		return true;
	}

	if (mGCObject.mShortcutCommand)
	{
		mGCObject.mShortcutCommand->DebugTick(InDeltaSecond);
	}
	if (mGCObject.mActorSelectManager)
	{
		mGCObject.mActorSelectManager->DebugTick(InDeltaSecond);
	}
	if (mGCObject.mDebugMenuManager)
	{
		mGCObject.mDebugMenuManager->DebugTick(InDeltaSecond);
	}
	if (mGCObject.mScreenWindowManager)
	{
		mGCObject.mScreenWindowManager->DebugTick(InDeltaSecond);
	}

	return true;
}
/**
 * @brief	Draw
 */
void	UCSDebug_Subsystem::DebugDraw(UCanvas* InCanvas, APlayerController* InPlayerController)
{
	if (mGCObject.mShortcutCommand)
	{
		mGCObject.mShortcutCommand->DebugDraw(InCanvas);
	}
	if (mGCObject.mActorSelectManager)
	{
		mGCObject.mActorSelectManager->DebugDraw(InCanvas);
	}
	if (mGCObject.mDebugMenuManager)
	{
		mGCObject.mDebugMenuManager->DebugDraw(InCanvas);
	}
	if (mGCObject.mScreenWindowManager)
	{
		mGCObject.mScreenWindowManager->DebugDraw(InCanvas);
	}
}
#endif