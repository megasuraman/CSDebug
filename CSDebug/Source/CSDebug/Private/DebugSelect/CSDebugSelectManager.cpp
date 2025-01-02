// Copyright 2020 SensyuGames.
/**
 * @file CSDebugSelectManager.cpp
 * @brief CSDebugSelect管理用Manager
 * @author SensyuGames
 * @date 2020/05/27
 */
#include "DebugSelect/CSDebugSelectManager.h"

#include "Engine/DebugCameraController.h"
#include "CollisionQueryParams.h"
#include "CSDebugSubsystem.h"
#include "DebugMenu/CSDebug_DebugMenuManager.h"
#include "DebugSelect/CSDebugSelectComponent.h"
#include "CSDebugConfig.h"
#include "DebugInfoWindow/CSDebugInfoWindowText.h"

#include "Engine/Canvas.h"
#include "CanvasItem.h"

#if USE_CSDEBUG
/**
 * @brief	Init
 */
void	UCSDebugSelectManager::Init()
{
 	UCSDebug_DebugMenuManager* DebugMenuManager = UCSDebug_DebugMenuManager::sGet(this);

 	const FString BaseDebugMenuPath(TEXT("CSDebug/DebugSelect"));
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath, FString(TEXT("Active")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath, FString(TEXT("UpdateOnlySelectActor")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("Info")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("Mark")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("Axis")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("Bone")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("PathFollow")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("LastEQS")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("BehaviorTree")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("Perception")), false);
}

/**
 * @brief	Tick
 */
bool	UCSDebugSelectManager::DebugTick(float InDeltaSecond)
{
	const UCSDebug_DebugMenuManager* DebugMenuManager = UCSDebug_DebugMenuManager::sGet(this);
	mbActive = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSDebug/DebugSelect/Active")));
	SetOnlyUpdateSelectActor(DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSDebug/DebugSelect/UpdateOnlySelectActor"))));
	mbShowInfo = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSDebug/DebugSelect/Draw/Info")));
	mbShowMark = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSDebug/DebugSelect/Draw/Mark")));
	mbShowSelectAxis = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSDebug/DebugSelect/Draw/Axis")));
	mbShowSelectBone = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSDebug/DebugSelect/Draw/Bone")));
	mbShowSelectPathFollow = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSDebug/DebugSelect/Draw/PathFollow")));
	mbShowSelectLastEQS = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSDebug/DebugSelect/Draw/LastEQS")));
	mbShowSelectBehaviorTree = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSDebug/DebugSelect/Draw/BehaviorTree")));
	mbShowSelectPerception = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSDebug/DebugSelect/Draw/Perception")));
	if (!mbActive)
	{
		return true;
	}
	CheckDebugCameraController();
	CheckSelectTarget();

	return true;
}

/**
 * @brief	Draw
 */
void	UCSDebugSelectManager::DebugDraw(UCanvas* InCanvas)
{
	if (!mbActive)
	{
		return;
	}
	DrawSelectMarker(InCanvas);

	if (mbShowInfo)
	{
		DrawInfo(InCanvas);
	}
	if (mbShowMark)
	{
		DrawMarkAllSelectList(InCanvas);
	}
	for (TWeakObjectPtr<UCSDebugSelectComponent> WeakPtr : mSelectList)
	{
		if (UCSDebugSelectComponent* DebugSelectComponent = WeakPtr.Get())
		{
			DebugSelectComponent->DebugDraw(InCanvas);
		}
	}
}

/**
 * @brief	UCSDebugSelectComponentの登録解除
 */
void	UCSDebugSelectManager::EntryDebugSelectComponent(UCSDebugSelectComponent* InComponent)
{
	mAllSelectList.Add(InComponent);

	if (AActor* Owner = InComponent->GetOwner())
	{
		SetActiveTickActor(Owner, !mbOnlyUpdateSelectActor);
	}
}
void	UCSDebugSelectManager::ExitDebugSelectComponent(UCSDebugSelectComponent* InComponent)
{
	mAllSelectList.RemoveSwap(InComponent);
}

/**
 * @brief	DebugCameraController監視
 */
void UCSDebugSelectManager::CheckDebugCameraController()
{
	if (mDebugCameraController.Get())
	{
		return;
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (ADebugCameraController* PlayerController = Cast<ADebugCameraController>(Iterator->Get()) )
		{
			mDebugCameraController = PlayerController;
			break;
		}
	}

#if 0//BindActionを使うほうが自然な気もするけど、プロジェクト設定でボタン変更したいのと、寿命の前後関係を気にしたくないので
	if (ADebugCameraController* DebugCameraController = mDebugCameraController.Get())
	{
		if (UInputComponent* InputComponent = DebugCameraController->InputComponent)
		{
			InputComponent->BindAction("DebugCamera_Select", IE_Pressed, this, &UCSDebugSelectManager::SelectTarget);
		}
		else
		{
			mDebugCameraController = nullptr;
		}
	}
#endif
}

/**
 * @brief	デバッグカメラ状態ならDebugCameraController取得
 */
ADebugCameraController* UCSDebugSelectManager::GetActiveDebugCameraController() const
{
	if(ADebugCameraController* DebugCameraController = mDebugCameraController.Get())
	{
		if (DebugCameraController->Player)
		{
			return DebugCameraController;
		}
	}

	return nullptr;
}

/**
 * @brief	対象選択監視
 */
void UCSDebugSelectManager::CheckSelectTarget()
{
	ADebugCameraController* DebugCameraController = GetActiveDebugCameraController();
	if (DebugCameraController == nullptr)
	{
		return;
	}

	const UPlayerInput* PlayerInput = DebugCameraController->PlayerInput;
	if (PlayerInput == nullptr)
	{
		return;
	}
	const UCSDebugConfig* CSDebugConfig = GetDefault<UCSDebugConfig>();
	if (CSDebugConfig->mDebugSelect_SelectKey.IsJustPressed(*PlayerInput))
	{
		FVector CamLoc;
		FRotator CamRot;
		DebugCameraController->GetPlayerViewPoint(CamLoc, CamRot);

		FHitResult HitResult;
		FCollisionQueryParams TraceParams(NAME_None, FCollisionQueryParams::GetUnknownStatId(), true, DebugCameraController);
		bool const bHit = DebugCameraController->GetWorld()->LineTraceSingleByChannel(HitResult, CamLoc, CamRot.Vector() * 5000.f * 20.f + CamLoc, ECC_Pawn, TraceParams);
		if (bHit)
		{
			if (AActor* HitActor = HitResult.Actor.Get())
			{
				OnSelect(HitActor);
			}
		}
	}
}

/**
 * @brief	対象選択時
 */
void UCSDebugSelectManager::OnSelect(AActor* InActor)
{
	UCSDebugSelectComponent* DebugSelectComponent = InActor->FindComponentByClass<UCSDebugSelectComponent>();
	if (DebugSelectComponent == nullptr)
	{
		return;
	}
	
	DebugSelectComponent->SetSelect(!DebugSelectComponent->IsSelect());

	if (DebugSelectComponent->IsSelect())
	{
		mSelectList.Add(DebugSelectComponent);

		if (AActor* Owner = DebugSelectComponent->GetOwner())
		{
			SetActiveTickActor(Owner, true);
		}
	}
	else
	{
		mSelectList.RemoveSwap(DebugSelectComponent);

		if (AActor* Owner = DebugSelectComponent->GetOwner())
		{
			SetActiveTickActor(Owner, !mbOnlyUpdateSelectActor);
		}
	}
}
/**
 * @brief	ActorのTickをon/off
 */
void UCSDebugSelectManager::SetActiveTickActor(AActor* InActor, const bool bInActive)
{
	InActor->SetActorTickEnabled(bInActive);
	for (auto Element : InActor->GetComponents())
	{
		Element->SetActive(bInActive);
	}

	if (APawn* Pawn = Cast<APawn>(InActor))
	{
		if (AController* Controller = Pawn->Controller)
		{
			SetActiveTickActor(Controller, bInActive);
		}
	}
}

/**
 * @brief	全体情報表示
 */
void	UCSDebugSelectManager::DrawInfo(UCanvas* InCanvas)
{
	FCSDebugInfoWindowText DebugInfoWindow;
	DebugInfoWindow.SetWindowName(FString(TEXT("CSDebugSelectManager")));
	DebugInfoWindow.AddText(FString::Printf(TEXT("AllSelectListNum : %d"), mAllSelectList.Num()));
	DebugInfoWindow.AddText(FString::Printf(TEXT("SelectListNum : %d"), mSelectList.Num()));
	DebugInfoWindow.Draw(InCanvas, FVector2D(InCanvas->ClipX*0.5f,50.f));
}
/**
 * @brief	画面中央の選択マーク
 */
void	UCSDebugSelectManager::DrawSelectMarker(UCanvas* InCanvas)
{
	ADebugCameraController* DebugCameraController = GetActiveDebugCameraController();
	if (DebugCameraController == nullptr)
	{
		return;
	}

	FVector CamLoc;
	FRotator CamRot;
	DebugCameraController->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector CenterPos(CamLoc + CamRot.Vector() * 5000.f * 20.f);
	//const FVector2D CenterPos2D(InCanvas->SizeX * 0.5f, InCanvas->SizeY * 0.5f);
	const FVector2D CenterPos2D( InCanvas->Project(CenterPos) );
	const float ExtentLen = 10.f;
	const FColor Color = FColor::Red;
	{
		FCanvasLineItem Item(FVector2D(CenterPos2D + FVector2D(ExtentLen)), FVector2D(CenterPos2D - FVector2D(ExtentLen)));
		Item.SetColor(Color);
		InCanvas->DrawItem(Item);
	}
	{
		FCanvasLineItem Item(FVector2D(CenterPos2D + FVector2D(ExtentLen,-ExtentLen)), FVector2D(CenterPos2D + FVector2D(-ExtentLen,ExtentLen)));
		Item.SetColor(Color);
		InCanvas->DrawItem(Item);
	}
}

/**
 * @brief	選択対象のマーク表示
 */
void	UCSDebugSelectManager::DrawMarkAllSelectList(UCanvas* InCanvas)
{
	for (TWeakObjectPtr<UCSDebugSelectComponent> WeakPtr : mAllSelectList)
	{
		if (const UCSDebugSelectComponent* DebugSelectComponent = WeakPtr.Get())
		{
			if (DebugSelectComponent->IsSelect())
			{
				continue;
			}
			DebugSelectComponent->DrawMark(InCanvas);
		}
	}
}

void	UCSDebugSelectManager::SetOnlyUpdateSelectActor(const bool bInOnlyUpdate)
{
	if (bInOnlyUpdate == mbOnlyUpdateSelectActor)
	{
		return;
	}
	mbOnlyUpdateSelectActor = bInOnlyUpdate;

	for (TWeakObjectPtr<UCSDebugSelectComponent> WeakPtr : mAllSelectList)
	{
		if (UCSDebugSelectComponent* DebugSelectComponent = WeakPtr.Get())
		{
			if (AActor* Owner = DebugSelectComponent->GetOwner())
			{
				const bool bActive = (!mbOnlyUpdateSelectActor || DebugSelectComponent->IsSelect());
				SetActiveTickActor(Owner, bActive);
			}
		}
	}
}

#endif//USE_CSDEBUG