// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_ActorSelectManager.h
 * @brief デバッグ選択管理用Manager
 * @author SensyuGames
 * @date 2020/05/27
 */
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebug_ActorSelectManager.generated.h"

class UCanvas;
class UCSDebug_ActorSelectComponent;
class ADebugCameraController;
struct FCSDebugMenuNodeGetter;
/**
 * 
 */
UCLASS()
class CSDEBUG_API UCSDebug_ActorSelectManager : public UObject
{
	GENERATED_BODY()

#if USE_CSDEBUG
public:
	static UCSDebug_ActorSelectManager* sGet(const UWorld* InWorld);

	void	Init();
	bool	DebugTick(float InDeltaSecond);
	void	DebugDraw(class UCanvas* InCanvas);
	void	EntryDebugSelectComponent(UCSDebug_ActorSelectComponent* InComponent);
	void	ExitDebugSelectComponent(UCSDebug_ActorSelectComponent* InComponent);

	bool	IsShowPathFollow() const { return mbShowSelectPathFollow; }
	bool	IsShowLastEQS() const { return mbShowSelectLastEQS; }

protected:
	ADebugCameraController* GetActiveDebugCameraController() const;
	void	CheckDebugCameraController();
	void	CheckSelectTarget();
	void	OnSelect(AActor* InActor);
	void	SetActiveTickActor(AActor* InActor, const bool bInActive);

	void	DrawInfo(UCanvas* InCanvas);
	void	DrawSelectMarker(UCanvas* InCanvas);
	void	DrawMarkAllSelectList(UCanvas* InCanvas);

	void	SetOnlyUpdateSelectActor(const bool bInOnlyUpdate);

private:
	TWeakObjectPtr<ADebugCameraController>	mDebugCameraController;
	TArray<TWeakObjectPtr<UCSDebug_ActorSelectComponent>>	mAllSelectList;
	TArray<TWeakObjectPtr<UCSDebug_ActorSelectComponent>>	mSelectList;
	bool	mbActive = false;
	bool	mbOnlyUpdateSelectActor = false;
	bool	mbShowInfo = false;
	bool	mbShowMark = false;
	bool	mbShowSelectAxis = false;
	bool	mbShowSelectBone = false;
	bool	mbShowSelectPathFollow = false;
	bool	mbShowSelectLastEQS = false;
	bool	mbShowSelectBehaviorTree = false;
	bool	mbShowSelectPerception = false;

#endif//USE_CSDEBUG
};
