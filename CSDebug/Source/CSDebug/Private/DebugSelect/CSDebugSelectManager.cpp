// Copyright 2020 SensyuGames.
/**
 * @file CSDebugSelectManager.cpp
 * @brief CSDebugSelect�Ǘ��pManager
 * @author SensyuGames
 * @date 2020/05/27
 */
#include "DebugSelect/CSDebugSelectManager.h"

#include "Engine/DebugCameraController.h"
#include "CollisionQueryParams.h"
#include "CSDebugSubsystem.h"
#include "DebugMenu/CSDebugMenuManager.h"
#include "DebugMenu/CSDebugMenuNodeGetter.h"
#include "DebugSelect/CSDebugSelectComponent.h"
#include "CSDebugConfig.h"
#include "DebugInfoWindow/CSDebugInfoWindowText.h"

#include "Engine/Canvas.h"
#include "CanvasItem.h"

/**
 * @brief	Init
 */
void	UCSDebugSelectManager::Init()
{
	UCSDebugMenuManager* CSDebugMenu = UCSDebugMenuManager::Get(this);

	const FString BaseDebugMenuPath(TEXT("CSDebug/DebugSelect/"));
	CSDebugMenu->AddNodePropertyBool(BaseDebugMenuPath + TEXT("Active"), mbActive);

	const auto& Delegate = FCSDebugMenuNodeDelegate::CreateUObject(this, &UCSDebugSelectManager::OnSetOnlyUpdateSelectActor);
	CSDebugMenu->AddValueNode(BaseDebugMenuPath + TEXT("UpdateOnlySelectActor"), mbOnlyUpdateSelectActor, Delegate);

	CSDebugMenu->AddNodePropertyBool(BaseDebugMenuPath + TEXT("Show/Info"), mbShowInfo);
	CSDebugMenu->AddNodePropertyBool(BaseDebugMenuPath + TEXT("Show/Mark"), mbShowMark);
	CSDebugMenu->AddNodePropertyBool(BaseDebugMenuPath + TEXT("ShowSelect/Bone"), mbShowSelectBone);
	CSDebugMenu->AddNodePropertyBool(BaseDebugMenuPath + TEXT("ShowSelect/PathFollow"), mbShowSelectPathFollow);
	CSDebugMenu->AddNodePropertyBool(BaseDebugMenuPath + TEXT("ShowSelect/BehaviorTree"), mbShowSelectBehaviorTree);
	CSDebugMenu->AddNodePropertyBool(BaseDebugMenuPath + TEXT("ShowSelect/EQS"), mbShowSelectLastEQS);
	CSDebugMenu->AddNodePropertyBool(BaseDebugMenuPath + TEXT("ShowSelect/Perctption"), mbShowSelectPerception);
}

/**
 * @brief	Tick
 */
bool	UCSDebugSelectManager::DebugTick(float InDeltaSecond)
{
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
 * @brief	UCSDebugSelectComponent�̓o�^����
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
 * @brief	DebugCameraController�Ď�
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

#if 0//BindAction���g���ق������R�ȋC�����邯�ǁA�v���W�F�N�g�ݒ�Ń{�^���ύX�������̂ƁA�����̑O��֌W���C�ɂ������Ȃ��̂�
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
 * @brief	�f�o�b�O�J������ԂȂ�DebugCameraController�擾
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
 * @brief	�ΏۑI���Ď�
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
	if (CSDebugConfig->mDebugSelect_SelectKey.IsJustPressed(PlayerInput))
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
 * @brief	�ΏۑI����
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
 * @brief	Actor��Tick��on/off
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
 * @brief	�S�̏��\��
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
 * @brief	��ʒ����̑I���}�[�N
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
 * @brief	�I��Ώۂ̃}�[�N�\��
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

//�f�o�b�O���j���[�Œl�ύX���̃R�[���o�b�N
void	UCSDebugSelectManager::OnSetOnlyUpdateSelectActor(const FCSDebugMenuNodeGetter& InGetter)
{
	mbOnlyUpdateSelectActor = InGetter.GetBool();

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