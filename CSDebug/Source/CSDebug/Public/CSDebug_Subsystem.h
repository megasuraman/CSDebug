// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_Subsystem.h
 * @brief CSDebug_Subsystem
 * @author SensyuGames
 * @date 2020/7/24
 */
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CSDebug_SaveData.h"
#include "CSDebug_Subsystem.generated.h"

class UCSDebug_ShortcutCommand;
class UCSDebug_ActorSelectManager;
class UCSDebugMenuManager;
class UCSDebugInfoWindowManager;
class UCSDebug_DebugMenuManager;

DECLARE_LOG_CATEGORY_EXTERN(CSDebugLog, Log, All);

/**
 * 
 */
UCLASS(BlueprintType)
class CSDEBUG_API UCSDebug_Subsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static FCSDebug_SaveData& sGetSaveData();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	UCSDebug_ScreenWindowManager* GetScreenWindowManagerBP() const;

	UCSDebug_ShortcutCommand* GetShortcutCommand() const { return mGCObject.mShortcutCommand; }
	UCSDebug_ActorSelectManager* GetActorSelectManager() const { return mGCObject.mActorSelectManager; }
	UCSDebug_DebugMenuManager* GetDebugMenuManager() const { return mGCObject.mDebugMenuManager; }
	UCSDebug_ScreenWindowManager* GetScreenWindowManager() const { return mGCObject.mScreenWindowManager; }

protected:
	void	RequestTick(const bool bInActive);
	void	RequestDraw(const bool bInActive);

	bool	DebugTick(float InDeltaSecond);
	void	DebugDraw(class UCanvas* InCanvas, class APlayerController* InPlayerController);

protected:
	struct FGCObjectCSDebug : public FGCObject
	{
		UCSDebug_ShortcutCommand* mShortcutCommand = nullptr;
		UCSDebug_ActorSelectManager* mActorSelectManager = nullptr;
		UCSDebug_DebugMenuManager* mDebugMenuManager = nullptr;
		UCSDebug_ScreenWindowManager* mScreenWindowManager = nullptr;
		virtual void AddReferencedObjects(FReferenceCollector& Collector) override
		{
			Collector.AddReferencedObject(mShortcutCommand);
			Collector.AddReferencedObject(mActorSelectManager);
			Collector.AddReferencedObject(mDebugMenuManager);
			Collector.AddReferencedObject(mScreenWindowManager);
		}
	};
	FGCObjectCSDebug	mGCObject;

private:
	TWeakObjectPtr<AActor>	mOwner;
	FDelegateHandle	mDebugTickHandle;
	FDelegateHandle	mDebugDrawHandle;
	static FCSDebug_SaveData mSaveData;
};
