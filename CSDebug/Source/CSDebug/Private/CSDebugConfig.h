// Copyright 2020 SensyuGames.
/**
 * @file CSDebugConfig.h
 * @brief CSDebug—p‚ÌConfig
 * @author SensyuGames
 * @date 2020/5/26
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebugConfig.generated.h"

class UCSDebugMenuWidgetBase;
class UCSDebugMenuListObjectBase;

USTRUCT(BlueprintType)
struct FCSDebugKey
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, config, Category = CSDebugCommand)
	FKey	mKeyboad;
	UPROPERTY(EditAnywhere, config, Category = CSDebugCommand)
	FKey	mPad;

	bool	IsPressed(const class UPlayerInput* InInput) const;
	bool	IsJustPressed(const class UPlayerInput* InInput) const;
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
class UCSDebugConfig : public UObject
{
	GENERATED_BODY()

	UCSDebugConfig();

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
	TSubclassOf<UCSDebugMenuWidgetBase>	mDebugMenu_WidgetClass;
	UPROPERTY(EditAnywhere, config, Category = CSDebugMenu)
	TSubclassOf<UCSDebugMenuListObjectBase>	mDebugMenu_ListObjectClass;
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
