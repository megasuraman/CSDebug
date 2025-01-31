// Copyright 2022 SensyuGames.
#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "CSDebug_EUW_Base.generated.h"

UCLASS()
class CSDEBUGEDITOR_API UCSDebug_EUW_Base : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "CSDebug_EUW")
	UWorld* GetWorld_Editor() const;
	UFUNCTION(BlueprintCallable, Category = "CSDebug_EUW")
	UWorld* GetWorld_GameServer() const;
	UFUNCTION(BlueprintCallable, Category = "CSDebug_EUW")
	UWorld* GetWorld_GameClient() const;
	UFUNCTION(BlueprintCallable, Category = "CSDebug_EUW")
	bool ParseCommandLine(FString& OutValue, const FString& InKey) const;
	UFUNCTION(BlueprintCallable, Category = "CSDebug_EUW")
	void SetActiveDraw(const bool bInDraw);
	UFUNCTION(BlueprintCallable, Category = "CSDebug_EUW")
	void OffAutoSaveFlag();
	UFUNCTION(BlueprintCallable, Category = "CSDebug_EUW")
	void RecoverAutoSaveFlag();
	UFUNCTION(BlueprintCallable, Category = "CSDebug_EUW")
	void FixupRedirects(const TArray<FString>& InTargetAssetNameList);
	UFUNCTION(BlueprintCallable, Category = "CSDebug_EUW")
	void SetRealTimeDraw_LevelEditorViewport(const bool bInActive) const;
	UFUNCTION(BlueprintCallable, Category = "CSDebug_EUW")
	void LookAtPos(FVector InPos);
	UFUNCTION(BlueprintCallable, Category = "CSDebug_EUW")
	void ClearAllSubLevel();
	
	virtual void FakeTick();
	virtual void OnRunGame(const UWorld& InWorld){}
	virtual void Draw(UCanvas* InCanvas, APlayerController* InPlayerController);

	
protected:
	FDelegateHandle mDebugDrawHandle;
	bool mbRunGameWorld = false;
	bool mbNeedRecoverAutoSaveFlag = false;
};
