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
	LookAtPos(mLastLookAtLocation);

	SetRealTimeDraw_LevelEditorViewport(true);
}

/**
 * @brief	表示on/off
 */
void	UCSDebug_EUW_ViewportHelper::RequestDraw(bool bInDraw)
{
	SetActiveDraw(bInDraw);
}

/**
 * @brief	描画
 */
void	UCSDebug_EUW_ViewportHelper::Draw(UCanvas* InCanvas, APlayerController* InPlayerController)
{
	Super::Draw(InCanvas, InPlayerController);

	if (GCurrentLevelEditingViewportClient)
	{
		const FVector LookAtPos = GCurrentLevelEditingViewportClient->GetLookAtLocation();
		FCSDebug_ScreenWindowText DebugInfo;
		DebugInfo.SetWindowName(FString(TEXT("ViewportHelper")));
		DebugInfo.AddText(FString::Printf(TEXT("LootAtPos : %s"), *LookAtPos.ToString()));
		DebugInfo.Draw(InCanvas, LookAtPos);
	}
}