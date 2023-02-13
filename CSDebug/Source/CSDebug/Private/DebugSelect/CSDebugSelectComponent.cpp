// Copyright 2020 SensyuGames.
/**
 * @file CSDebugSelectComponent.cpp
 * @brief CSDebugSelect‘ÎÛ—pComponent
 * @author SensyuGames
 * @date 2020/05/27
 */

#include "DebugSelect/CSDebugSelectComponent.h"
#include "DebugSelect/CSDebugSelectManager.h"
#include "CSDebugSubsystem.h"
#include "CSDebugDraw.h"

#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "AIModule/Classes/AIController.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UCSDebugSelectComponent::UCSDebugSelectComponent()
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
void UCSDebugSelectComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const AActor* OwnerActor = GetOwner())
	{
		mDebugInfoWindow.SetWindowName(FString::Printf(TEXT("%s"), *OwnerActor->GetName()));
	}

#if USE_CSDEBUG
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	UCSDebugSubsystem* CSDebugSubsystem = GameInstance->GetSubsystem<UCSDebugSubsystem>();
	UCSDebugSelectManager* CSDebugSelectManager = CSDebugSubsystem->GetDebugSelectManager();
	CSDebugSelectManager->EntryDebugSelectComponent(this);
	mManager = CSDebugSelectManager;
#endif
}

void UCSDebugSelectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UCSDebugSelectManager* CSDebugSelectManager = mManager.Get())
	{
		CSDebugSelectManager->ExitDebugSelectComponent(this);
	}
}

/**
 * @brief	‘I‘ðó‘ÔÝ’è
 */
void UCSDebugSelectComponent::SetSelect(bool bInSelect)
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
void	UCSDebugSelectComponent::DebugDraw(UCanvas* InCanvas)
{
	const UCSDebugSelectManager* CSDebugSelectManager = mManager.Get();
	const AActor* OwnerActor = GetOwner();
	if (CSDebugSelectManager == nullptr
		|| OwnerActor == nullptr)
	{
		return;
	}

	if (mbUsePreDrawDelegate)
	{
		mPreDrawDelegate.Execute();
	}

	mDebugInfoWindow.FittingWindowExtent(InCanvas);
	mDebugInfoWindow.SetWindowFrameColor(GetMyColor());
	mDebugInfoWindow.Draw(InCanvas, OwnerActor->GetActorLocation());

	mDebugInfoWindow.ClearString();

	if (const APawn* Pawn = Cast<APawn>(OwnerActor))
	{
		if (const AAIController* AIController = Cast<AAIController>(Pawn->Controller))
		{
 			if (CSDebugSelectManager->IsShowPathFollow())
 			{
				UCSDebugDraw::DrawPathFollowRoute(GetWorld(), InCanvas, AIController, true);
 			}
 			if (CSDebugSelectManager->IsShowLastEQS())
 			{
 				UCSDebugDraw::DrawLastEQS(GetWorld(), InCanvas, AIController);
 			}
		}
	}
}

/**
 * @brief	DrawMark
 */
void	UCSDebugSelectComponent::DrawMark(UCanvas* InCanvas) const
{
	const AActor* OwnerActor = GetOwner();
	if (OwnerActor == nullptr)
	{
		return;
	}
	
	const FVector OwnerPos = OwnerActor->GetActorLocation();
	const FVector2D ExtentV(5.f, 5.f);
	UCSDebugDraw::DrawCanvasQuadrangle(InCanvas, OwnerPos, ExtentV, GetMyColor());
}
#endif//USE_CSDEBUG