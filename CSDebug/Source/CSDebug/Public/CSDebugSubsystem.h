// Copyright 2020 SensyuGames.
/**
 * @file CSDebugSubsystem.h
 * @brief CSDebug—p‚ÌSubsystem
 * @author SensyuGames
 * @date 2020/7/24
 */
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CSDebugSubsystem.generated.h"

class UCSDebugCommand;
class UCSDebugSelectManager;
class UCSDebugMenuManager;
class UCSDebugInfoWindowManager;

/**
 * 
 */
UCLASS(BlueprintType)
class CSDEBUG_API UCSDebugSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UCSDebugCommand* GetDebugCommand() const { return mDebugCommand; }
	UCSDebugSelectManager* GetDebugSelectManager() const { return mDebugSelectManager; }
	UCSDebugMenuManager* GetDebugMenuManager() const { return mDebugMenuManager; }
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	UCSDebugInfoWindowManager* GetDebugInfoWindowManager() const { return mDebugInfoWindowManager; }

protected:
	void	RequestTick(const bool bInActive);
	void	RequestDraw(const bool bInActive);

	bool	DebugTick(float InDeltaSecond);
	void	DebugDraw(class UCanvas* InCanvas, class APlayerController* InPlayerController);

protected:
	UPROPERTY()
	UCSDebugCommand*	mDebugCommand = nullptr;
	UPROPERTY()
	UCSDebugSelectManager*	mDebugSelectManager = nullptr;
	UPROPERTY(BlueprintReadOnly)
	UCSDebugMenuManager*	mDebugMenuManager = nullptr;
	UPROPERTY()
	UCSDebugInfoWindowManager*	mDebugInfoWindowManager = nullptr;

private:
	TWeakObjectPtr<AActor>	mOwner;
	FDelegateHandle	mDebugTickHandle;
	FDelegateHandle	mDebugDrawHandle;
};
