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
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	UCSDebugMenuManager* GetDebugMenuManagerBP() const;
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	UCSDebugInfoWindowManager* GetDebugInfoWindowManagerBP() const;

#if USE_CSDEBUG
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UCSDebugCommand* GetDebugCommand() const { return mGCObject.mDebugCommand; }
	UCSDebugSelectManager* GetDebugSelectManager() const { return mGCObject.mDebugSelectManager; }
	UCSDebugMenuManager* GetDebugMenuManager() const { return mGCObject.mDebugMenuManager; }
	UCSDebugInfoWindowManager* GetDebugInfoWindowManager() const { return mGCObject.mDebugInfoWindowManager; }

protected:
	void	RequestTick(const bool bInActive);
	void	RequestDraw(const bool bInActive);

	bool	DebugTick(float InDeltaSecond);
	void	DebugDraw(class UCanvas* InCanvas, class APlayerController* InPlayerController);

protected:
	struct FGCObjectCSDebug : public FGCObject
	{
		UCSDebugCommand* mDebugCommand = nullptr;
		UCSDebugSelectManager* mDebugSelectManager = nullptr;
		UCSDebugMenuManager* mDebugMenuManager = nullptr;
		UCSDebugInfoWindowManager* mDebugInfoWindowManager = nullptr;
		virtual void AddReferencedObjects(FReferenceCollector& Collector) override
		{
			Collector.AddReferencedObject(mDebugCommand);
			Collector.AddReferencedObject(mDebugSelectManager);
			Collector.AddReferencedObject(mDebugMenuManager);
			Collector.AddReferencedObject(mDebugInfoWindowManager);
		}
	};
	FGCObjectCSDebug	mGCObject;

private:
	TWeakObjectPtr<AActor>	mOwner;
	FDelegateHandle	mDebugTickHandle;
	FDelegateHandle	mDebugDrawHandle;
#endif//USE_CSDEBUG
};
