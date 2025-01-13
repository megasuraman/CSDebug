// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_ActorSelectComponent.cpp
 * @brief デバッグ選択対象Component
 * @author SensyuGames
 * @date 2020/05/27
 */

#include "ActorSelect/CSDebug_ActorSelectComponent.h"
#include "ActorSelect/CSDebug_ActorSelectManager.h"
#include "CSDebug_Subsystem.h"
#include "CSDebug_Draw.h"

#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "AIModule/Classes/AIController.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UCSDebug_ActorSelectComponent::UCSDebug_ActorSelectComponent()
#if USE_CSDEBUG
	:mbSelect(false)
	,mbUsePreDrawDelegate(false)
#endif//USE_CSDEBUG
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


#if USE_CSDEBUG
// Called when the game starts
void UCSDebug_ActorSelectComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const AActor* OwnerActor = GetOwner())
	{
		mScreenWindow.SetWindowName(FString::Printf(TEXT("%s"), *OwnerActor->GetName()));
	}

#if USE_CSDEBUG
	UCSDebug_ActorSelectManager* ActorSelectManager = UCSDebug_ActorSelectManager::sGet(GetWorld());
	ActorSelectManager->EntryDebugSelectComponent(this);
	mManager = ActorSelectManager;
#endif
}

void UCSDebug_ActorSelectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UCSDebug_ActorSelectManager* ActorSelectManager = mManager.Get())
	{
		ActorSelectManager->ExitDebugSelectComponent(this);
	}
}

/**
 * @brief	選択状態設定
 */
void UCSDebug_ActorSelectComponent::SetSelect(bool bInSelect)
{
	if (mbSelect == bInSelect)
	{
		return;
	}

	mbSelect = bInSelect;
}

/**
 * @brief	Draw
 */
void	UCSDebug_ActorSelectComponent::DebugDraw(UCanvas* InCanvas)
{
	const UCSDebug_ActorSelectManager* ActorSelectManager = mManager.Get();
	const AActor* OwnerActor = GetOwner();
	if (ActorSelectManager == nullptr
		|| OwnerActor == nullptr)
	{
		return;
	}

	if (mbUsePreDrawDelegate)
	{
		mPreDrawDelegate.Execute();
	}

	mScreenWindow.FittingWindowExtent(InCanvas);
	mScreenWindow.SetWindowFrameColor(GetMyColor());
	mScreenWindow.Draw(InCanvas, OwnerActor->GetActorLocation());

	mScreenWindow.ClearString();

	if (const APawn* Pawn = Cast<APawn>(OwnerActor))
	{
		if (const AAIController* AIController = Cast<AAIController>(Pawn->Controller))
		{
 			if (ActorSelectManager->IsShowPathFollow())
 			{
				UCSDebug_Draw::DrawPathFollowRoute(GetWorld(), InCanvas, AIController, true);
 			}
 			if (ActorSelectManager->IsShowLastEQS())
 			{
 				UCSDebug_Draw::DrawLastEQS(GetWorld(), InCanvas, AIController);
 			}
		}
	}
}

/**
 * @brief	DrawMark
 */
void	UCSDebug_ActorSelectComponent::DrawMark(UCanvas* InCanvas) const
{
	const AActor* OwnerActor = GetOwner();
	if (OwnerActor == nullptr)
	{
		return;
	}
	
	const FVector OwnerPos = OwnerActor->GetActorLocation();
	const FVector2D ExtentV(5.f, 5.f);
	UCSDebug_Draw::DrawCanvasQuadrangle(InCanvas, OwnerPos, ExtentV, GetMyColor());
}
#endif//USE_CSDEBUG