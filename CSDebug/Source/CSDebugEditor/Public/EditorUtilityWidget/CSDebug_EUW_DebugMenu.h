// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_EUW_DebugMenu.h
 * @brief DebugMenu操作EUW
 * @author SensyuGames
 * @date 2025/01/27
 */

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget/CSDebug_EUW_Base.h"
#include "CSDebug_EUW_DebugMenu.generated.h"

/**
 * 
 */
UCLASS()
class CSDEBUGEDITOR_API UCSDebug_EUW_DebugMenu : public UCSDebug_EUW_Base
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	virtual void OnRunGame(const UWorld& InWorld) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CSDebug_EUW_DebugMenu", DisplayName = "AutoLoad")
	bool mbAutoLoad = false;
};
