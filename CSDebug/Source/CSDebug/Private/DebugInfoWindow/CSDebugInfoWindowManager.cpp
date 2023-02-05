// Copyright 2020 SensyuGames.
/**
 * @file CSDebugInfoWindowManager.cpp
 * @brief DebugInfoWindow管理用のManager
 * @author SensyuGames
 * @date 2023/02/04
 */
#include "DebugInfoWindow/CSDebugInfoWindowManager.h"

#include "CSDebugConfig.h"
#include "DebugInfoWindow/CSDebugInfoWindowText.h"
#include "CSDebugSubsystem.h"


UCSDebugInfoWindowManager::UCSDebugInfoWindowManager()
{
	mTempWindowDataList.Reserve(32);
}

void    UCSDebugInfoWindowManager::BeginDestroy()
{
	Super::BeginDestroy();
}

/**
 * @brief	Get
 */
UCSDebugInfoWindowManager* UCSDebugInfoWindowManager::Get(UObject* InOwner)
{
	UGameInstance* GameInstance = InOwner->GetWorld()->GetGameInstance();
	UCSDebugSubsystem* CSDebugSubsystem = GameInstance->GetSubsystem<UCSDebugSubsystem>();
	return CSDebugSubsystem->GetDebugInfoWindowManager();
}

/**
 * @brief	Init
 */
void	UCSDebugInfoWindowManager::Init()
{
}

/**
 * @brief	Tick
 */
bool	UCSDebugInfoWindowManager::DebugTick(float InDeltaSecond)
{
	UpdateLifeTime(InDeltaSecond);
	return true;
}

/**
 * @brief	Draw
 */
void	UCSDebugInfoWindowManager::DebugDraw(UCanvas* InCanvas)
{
	DrawWindow(InCanvas);
}

/**
 * @brief	Window追加
 */
void	UCSDebugInfoWindowManager::AddWindow(const FName InTag, const FString& InMessage, const AActor* InFollowActor, const FCSDebugInfoWindowOption& InOption)
{
	OnAddWindow(InTag, InMessage, InFollowActor, InOption);
}
void	UCSDebugInfoWindowManager::AddWindowBP(const FName InTag, const FText InMessage, const AActor* InFollowActor, float InDispTime)
{
	FCSDebugInfoWindowOption Option;
	Option.mDispTime = InDispTime;
	OnAddWindow(InTag, InMessage.ToString(), InFollowActor, Option);
}
/**
 * @brief	Window追加
 */
void	UCSDebugInfoWindowManager::OnAddWindow(const FName InTag, const FString& InMessage, const AActor* InFollowActor, const FCSDebugInfoWindowOption& InOption)
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
void UCSDebugInfoWindowManager::UpdateLifeTime(const float InDeltaSecond)
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
void UCSDebugInfoWindowManager::DrawWindow(UCanvas* InCanvas)
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
