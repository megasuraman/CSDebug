// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_ScreenWindowManager.cpp
 * @brief DebugInfoWindow管理用のManager
 * @author SensyuGames
 * @date 2023/02/04
 */
#include "ScreenWindow/CSDebug_ScreenWindowManager.h"

#include "ScreenWindow/CSDebug_ScreenWindowText.h"
#include "CSDebug_Subsystem.h"


UCSDebug_ScreenWindowManager::UCSDebug_ScreenWindowManager()
{
#if USE_CSDEBUG
	mTempWindowDataList.Reserve(32);
#endif//USE_CSDEBUG
}

/**
 * @brief	Window追加
 */
void	UCSDebug_ScreenWindowManager::AddWindowBP(const FName InTag, const FText InMessage, const AActor* InFollowActor, float InDispTime)
{
#if USE_CSDEBUG
	FCSDebug_ScreenWindowOption Option;
	Option.mDispTime = InDispTime;
	OnAddWindow(InTag, InMessage.ToString(), InFollowActor, Option);
#endif//USE_CSDEBUG
}

#if USE_CSDEBUG
/**
 * @brief	Get
 */
UCSDebug_ScreenWindowManager* UCSDebug_ScreenWindowManager::Get(UObject* InOwner)
{
	UGameInstance* GameInstance = InOwner->GetWorld()->GetGameInstance();
	UCSDebug_Subsystem* CSDebugSubsystem = GameInstance->GetSubsystem<UCSDebug_Subsystem>();
	return CSDebugSubsystem->GetScreenWindowManager();
}

/**
 * @brief	Init
 */
void	UCSDebug_ScreenWindowManager::Init()
{
}

/**
 * @brief	Tick
 */
bool	UCSDebug_ScreenWindowManager::DebugTick(float InDeltaSecond)
{
	UpdateLifeTime(InDeltaSecond);
	return true;
}

/**
 * @brief	Draw
 */
void	UCSDebug_ScreenWindowManager::DebugDraw(UCanvas* InCanvas)
{
	DrawWindow(InCanvas);
}

/**
 * @brief	Window追加
 */
void	UCSDebug_ScreenWindowManager::AddWindow(const FName InTag, const FString& InMessage, const AActor* InFollowActor, const FCSDebug_ScreenWindowOption& InOption)
{
	OnAddWindow(InTag, InMessage, InFollowActor, InOption);
}
/**
 * @brief	Window追加
 */
void	UCSDebug_ScreenWindowManager::OnAddWindow(const FName InTag, const FString& InMessage, const AActor* InFollowActor, const FCSDebug_ScreenWindowOption& InOption)
{
	for (FTempWindowData& Data : mTempWindowDataList)
	{
		if (Data.mTagName == InTag)
		{
			Data.mLifeTime = InOption.mDispTime;
			Data.mWindow.SetWindowName(InTag.ToString());
			Data.mWindow.ClearString();
			Data.mWindow.AddText(InMessage);
			Data.mWindow.SetWindowFrameColor(InOption.mFrameColor);
			Data.mFollowTarget = InFollowActor;
			Data.mbActive = true;
			return;
		}
	}

	if (mTempWindowDataList.Num() >= 256)
	{
		return;
	}

	FTempWindowData Data;
	Data.mTagName = InTag;
	Data.mLifeTime = InOption.mDispTime;
	Data.mWindow.SetWindowName(InTag.ToString());
	Data.mWindow.ClearString();
	Data.mWindow.AddText(InMessage);
	Data.mWindow.SetWindowFrameColor(InOption.mFrameColor);
	Data.mFollowTarget = InFollowActor;
	Data.mbActive = true;
	mTempWindowDataList.Add(Data);
}

/**
 * @brief	寿命更新
 */
void UCSDebug_ScreenWindowManager::UpdateLifeTime(const float InDeltaSecond)
{
	for (FTempWindowData& Data : mTempWindowDataList)
	{
		if (Data.mbActive)
		{
			Data.mLifeTime -= InDeltaSecond;
			if (Data.mLifeTime <= 0.f)
			{
				Data.mbActive = false;
			}
		}
	}
}

/**
 * @brief	Window表示
 */
void UCSDebug_ScreenWindowManager::DrawWindow(UCanvas* InCanvas)
{
	FVector2D DispPos(30.f, 30.f);
	for (FTempWindowData& Data : mTempWindowDataList)
	{
		if (!Data.mbActive)
		{
			continue;
		}
		if (const AActor* FollowActor = Data.mFollowTarget.Get())
		{
			Data.mWindow.Draw(InCanvas, FollowActor->GetActorLocation());
		}
		else
		{
			const FVector2D WindowExtent = Data.mWindow.Draw(InCanvas, DispPos);
			DispPos.X += WindowExtent.X + 10.f;
		}
	}
}

#endif//USE_CSDEBUG