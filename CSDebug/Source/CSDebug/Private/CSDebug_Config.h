// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_Config.h
 * @brief CSDebug—p‚ÌConfig
 * @author SensyuGames
 * @date 2020/5/26
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebug_Config.generated.h"

USTRUCT(BlueprintType)
struct FCSDebugKey
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, config, Category = CSDebugCommand)
	FKey	mKeyboad;
	UPROPERTY(EditAnywhere, config, Category = CSDebugCommand)
	FKey	mPad;

	bool	IsPressed(const class UPlayerInput& InInput) const;
	bool	IsJustPressed(const class UPlayerInput& InInput) const;
	bool	IsJustReleased(const class UPlayerInput& InInput) const;
};

USTRUCT(BlueprintType)
struct FCSDebugSecretCommand
{
	GENERATED_USTRUCT_BODY();
	
	UPROPERTY(EditAnywhere, config, Category = CSDebugCommand)
	TArray<FKey> mKeyList;
};

/**
 * 
 */
UCLASS(config = CSDebug, defaultconfig)
class UCSDebug_Config : public UObject
{
	GENERATED_BODY()

	UCSDebug_Config();

public:
	UPROPERTY(EditAnywhere, config, Category = CSDebug)
	bool	mbActiveCSDebug = true;

	UPROPERTY(EditAnywhere, config, Category = CSDebugCommand)
	FCSDebugKey	mDebugCommand_ReadyKey;
	
	UPROPERTY(EditAnywhere, config, Category = CSDebugCommand)
	FCSDebugKey	mDebugCommand_DebugMenuKey;
	
	UPROPERTY(EditAnywhere, config, Category = CSDebugCommand)
	FCSDebugKey	mDebugCommand_DebugStopKey;
	
	UPROPERTY(EditAnywhere, config, Category = CSDebugCommand)
	FCSDebugKey	mDebugCommand_DebugCameraKey;

	UPROPERTY(EditAnywhere, config, Category = CSDebugCommand)
	TMap<FString, FCSDebugSecretCommand> mDebugSecretCommand;
	
	UPROPERTY(EditAnywhere, config, Category = CSDebugSelect)
	FCSDebugKey	mDebugSelect_SelectKey;

	UPROPERTY(EditAnywhere, config, Category = CSDebugMenu)
	TSubclassOf<class UCSDebug_DebugMenuManager> mDebugMenuManagerClass;
	UPROPERTY(EditAnywhere, config, Category = CSDebugMenu)
	TSoftObjectPtr<class UDataTable> mDebugMenuDataTable = nullptr;
	UPROPERTY(EditAnywhere, config, Category = CSDebugMenu)
	FCSDebugKey	mDebugMenu_SelectKey;
	UPROPERTY(EditAnywhere, config, Category = CSDebugMenu)
	FCSDebugKey	mDebugMenu_CancelKey;
	UPROPERTY(EditAnywhere, config, Category = CSDebugMenu)
	FCSDebugKey	mDebugMenu_UpKey;
	UPROPERTY(EditAnywhere, config, Category = CSDebugMenu)
	FCSDebugKey	mDebugMenu_DownKey;
	UPROPERTY(EditAnywhere, config, Category = CSDebugMenu)
	FCSDebugKey	mDebugMenu_RightKey;
	UPROPERTY(EditAnywhere, config, Category = CSDebugMenu)
	FCSDebugKey	mDebugMenu_LeftKey;
};
