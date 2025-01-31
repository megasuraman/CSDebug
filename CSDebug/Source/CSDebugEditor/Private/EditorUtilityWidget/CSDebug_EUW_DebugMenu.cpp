// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_EUW_DebugMenu.cpp
 * @brief DebugMenu操作EUW
 * @author SensyuGames
 * @date 2025/01/27
 */

#include "EditorUtilityWidget/CSDebug_EUW_DebugMenu.h"

#include "CSDebug_Subsystem.h"

bool UCSDebug_EUW_DebugMenu::Initialize()
{
	const bool bResult = Super::Initialize();

	mbAutoLoad = UCSDebug_Subsystem::sGetSaveData().GetBool(FString(TEXT("DebugMenu_AutoLoad")));

	return bResult;
}

void UCSDebug_EUW_DebugMenu::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UCSDebug_Subsystem::sGetSaveData().SetBool(FString(TEXT("DebugMenu_AutoLoad")), mbAutoLoad);
}

void UCSDebug_EUW_DebugMenu::OnRunGame(const UWorld& InWorld)
{

}
