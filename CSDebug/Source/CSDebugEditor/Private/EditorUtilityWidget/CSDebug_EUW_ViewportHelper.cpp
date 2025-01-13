// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_EUW_ViewportHelper.cpp
 * @brief UObjectのProperty値表示
 * @author SensyuGames
 * @date 2022/08/07
 */


#include "EditorUtilityWidget/CSDebug_EUW_ViewportHelper.h"
#include "Editor.h"
#include "EditorViewportClient.h"
#include "LevelEditor.h"
#include "SLevelViewport.h"
#include "Debug/DebugDrawService.h"
#include "ScreenWindow/CSDebug_ScreenWindowText.h"
#include "LevelEditorViewport.h"

/**
 * @brief	カメラを指定座標に向ける
 */
void	UCSDebug_EUW_ViewportHelper::LookAt(const FString& InString)
{
	mLastLookAtLocation.InitFromString(InString);
	if (GCurrentLevelEditingViewportClient != nullptr)
	{
		GCurrentLevelEditingViewportClient->SetViewLocation(mLastLookAtLocation + FVector(500.f));
		GCurrentLevelEditingViewportClient->SetLookAtLocation(mLastLookAtLocation, true);
	}

	if (GCurrentLevelEditingViewportClient)
	{
		GCurrentLevelEditingViewportClient->SetViewLocation(mLastLookAtLocation + FVector(500.f));
		GCurrentLevelEditingViewportClient->SetLookAtLocation(mLastLookAtLocation, true);

		//ViewportのRealTime表示を有効に
		FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
		TSharedPtr<SLevelViewport> ActiveLevelViewport = LevelEditorModule.GetFirstActiveLevelViewport();
		if (ActiveLevelViewport.IsValid())
		{
			FLevelEditorViewportClient& LevelViewportClient = ActiveLevelViewport->GetLevelViewportClient();
			LevelViewportClient.PopRealtimeOverride();
			LevelViewportClient.SetRealtime(true);
		}
	}
}

/**
 * @brief	表示on/off
 */
void	UCSDebug_EUW_ViewportHelper::RequestDraw(bool bInDraw)
{
	if (bInDraw
		&& !mDebugDrawHandle.IsValid())
	{
		FDebugDrawDelegate DrawDebugDelegate = FDebugDrawDelegate::CreateUObject(this, &UCSDebug_EUW_ViewportHelper::Draw);
		mDebugDrawHandle = UDebugDrawService::Register(TEXT("GameplayDebug"), DrawDebugDelegate);
	}
	else if (!bInDraw
			&& mDebugDrawHandle.IsValid())
	{
		UDebugDrawService::Unregister(mDebugDrawHandle);
		mDebugDrawHandle.Reset();
	}
}

/**
 * @brief	Widget閉じたとき
 */
void	UCSDebug_EUW_ViewportHelper::NativeDestruct()
{
	Super::NativeDestruct();

	RequestDraw(false);
}

/**
 * @brief	描画
 */
void	UCSDebug_EUW_ViewportHelper::Draw(UCanvas* InCanvas, APlayerController* InPlayerController)
{
	if (GCurrentLevelEditingViewportClient)
	{
		const FVector LookAtPos = GCurrentLevelEditingViewportClient->GetLookAtLocation();
		FCSDebug_ScreenWindowText DebugInfo;
		DebugInfo.SetWindowName(FString(TEXT("ViewportHelper")));
		DebugInfo.AddText(FString::Printf(TEXT("LootAtPos : %s"), *LookAtPos.ToString()));
		DebugInfo.Draw(InCanvas, LookAtPos);
	}
}