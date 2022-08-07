// Copyright 2020 SensyuGames.
/**
 * @file CSDebugSubsystem.h
 * @brief CSDebug用のSubsystem
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

/**
 * 
 */
UCLASS()
class CSDEBUG_API UCSDebugSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

#if USE_CSDEBUG//ビルドエラーで気づけるように封印
	UCSDebugCommand* GetDebugCommand() const { return mDebugCommand; }
	UCSDebugSelectManager* GetDebugSelectManager() const { return mDebugSelectManager; }
	UCSDebugMenuManager* GetDebugMenuManager() const { return mDebugMenuManager; }
#endif //USE_CSDEBUG

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

private:
	TWeakObjectPtr<AActor>	mOwner;
	FDelegateHandle	mDebugTickHandle;
	FDelegateHandle	mDebugDrawHandle;
};
