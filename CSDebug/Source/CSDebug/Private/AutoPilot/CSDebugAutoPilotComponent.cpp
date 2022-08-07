// Copyright 2020 SensyuGames.
/**
 * @file CSDebugAutoPilotComponent.cpp
 * @brief PlayerController�ɑ΂��Ď������͓���⏕����Component
 * @author SensyuGames
 * @date 2021/12/28
 */


#include "AutoPilot/CSDebugAutoPilotComponent.h"
#include "AutoPilot/CSDebugAutoPilotModeRecord.h"

#include "Kismet/GameplayStatics.h"
#include "Debug/DebugDrawService.h"
#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"

// Sets default values for this component's properties
UCSDebugAutoPilotComponent::UCSDebugAutoPilotComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCSDebugAutoPilotComponent::BeginPlay()
{
	Super::BeginPlay();

	RequestDebugDraw(true);
}
void UCSDebugAutoPilotComponent::BeginDestroy()
{
	Super::BeginDestroy();

	RequestDebugDraw(false);
 	SetFixFrameRate(false);
 	SetIgnoreDefaultInput(false);
}

/**
 * @brief PlayerInput�̏����O
 */
void UCSDebugAutoPilotComponent::PreProcessInput(float DeltaTime)
{
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		return;
	}

 	if (mActiveMode)
 	{
 		mActiveMode->PreProcessInput(DeltaTime);
 	}
}

/**
 * @brief PlayerInput�̏�����
 */
void UCSDebugAutoPilotComponent::PostProcessInput(float DeltaTime)
{
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		return;
	}

 	if (mActiveMode)
 	{
 		mActiveMode->PostProcessInput(DeltaTime);
 	}
}

/**
 * @brief APlayerController���擾
 */
APlayerController* UCSDebugAutoPilotComponent::GetPlayerController()
{
	return Cast<APlayerController>(GetOwner());
}

/**
 * @brief Mode�Z�b�g
 */
void UCSDebugAutoPilotComponent::SetMode(ECSDebugAutoPilotMode InMode)
{
	if (mMode == InMode)
	{
		return;
	}

	OnEndMode();

	mMode = InMode;
	if (mActiveMode != nullptr)
	{
		mActiveMode->MarkPendingKill();
		mActiveMode = nullptr;
	}


	switch (mMode)
	{
	case ECSDebugAutoPilotMode::Record:
		mActiveMode = NewObject<UCSDebugAutoPilotModeBase>(this, UCSDebugAutoPilotModeRecord::StaticClass());
		break;
	default:
		break;
	}

	if (mActiveMode)
	{
		mActiveMode->SetParent(this);
	}

	OnBeginMode();
}

/**
 * @brief �ʏ�̓��͏����̖�����
 */
void UCSDebugAutoPilotComponent::SetIgnoreDefaultInput(const bool InIgnore)
{
	if (mbIgnoreInput != InIgnore)
	{
		mbIgnoreInput = InIgnore;
		if (UGameViewportClient* GameViewportClient = GetWorld()->GetGameViewport())
		{
			GameViewportClient->SetIgnoreInput(mbIgnoreInput);
		}
	}
}

/* ------------------------------------------------------------
   !�Đ�
------------------------------------------------------------ */
/**
 * @brief 
 */
void UCSDebugAutoPilotComponent::RequestPlayInputRecord(const FString& InFileName)
{
	SetMode(ECSDebugAutoPilotMode::Record);
	UCSDebugAutoPilotModeRecord* ModeRecord = Cast<UCSDebugAutoPilotModeRecord>(mActiveMode);
	ModeRecord->RequestPlayInputRecord(InFileName);
}

/**
 * @brief �Đ��I��������ǂ���
 */
bool UCSDebugAutoPilotComponent::IsFinishPlayRecord() const
{
	if (const UCSDebugAutoPilotModeRecord* ModeRecord = Cast<UCSDebugAutoPilotModeRecord>(mActiveMode))
	{
		return ModeRecord->IsFinihPlay();
	}
	return false;
}

/**
 * @brief �R�}���h���^�J�n
 */
void UCSDebugAutoPilotComponent::RequestBeginRecord(const FString& InFileName)
{
	SetMode(ECSDebugAutoPilotMode::Record);
	UCSDebugAutoPilotModeRecord* ModeRecord = Cast<UCSDebugAutoPilotModeRecord>(mActiveMode);
	ModeRecord->RequestBeginRecord(InFileName);
}

/**
 * @brief �R�}���h���^�I��
 */
void UCSDebugAutoPilotComponent::RequestEndRecord()
{
	SetMode(ECSDebugAutoPilotMode::Record);
	UCSDebugAutoPilotModeRecord* ModeRecord = Cast<UCSDebugAutoPilotModeRecord>(mActiveMode);
	ModeRecord->RequestEndRecord();
}

/**
 * @brief �R�}���h���^��Idle���
 */
void UCSDebugAutoPilotComponent::RequestIdleRecord()
{
	SetMode(ECSDebugAutoPilotMode::Record);
	UCSDebugAutoPilotModeRecord* ModeRecord = Cast<UCSDebugAutoPilotModeRecord>(mActiveMode);
	ModeRecord->RequestIdle();
}

/**
 * @brief	Draw�o�^��on/off
 */
void	UCSDebugAutoPilotComponent::RequestDebugDraw(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugDrawHandle.IsValid())
		{
			auto DebugDrawDelegate = FDebugDrawDelegate::CreateUObject(this, &UCSDebugAutoPilotComponent::DebugDraw);
			if (DebugDrawDelegate.IsBound())
			{
				mDebugDrawHandle = UDebugDrawService::Register(TEXT("GameplayDebug"), DebugDrawDelegate);
			}
		}
	}
	else
	{
		if (mDebugDrawHandle.IsValid())
		{
			UDebugDrawService::Unregister(mDebugDrawHandle);
			mDebugDrawHandle.Reset();
		}
	}
}

/**
 * @brief	Draw
 */
void	UCSDebugAutoPilotComponent::DebugDraw(UCanvas* InCanvas, APlayerController* InPlayerController)
{
	if (mActiveMode)
	{
		mActiveMode->DebugDraw(InCanvas);
	}
}

/**
 * @brief	���[�h�J�n��
 */
void UCSDebugAutoPilotComponent::OnBeginMode()
{
	switch (mMode)
	{
	case ECSDebugAutoPilotMode::Record:
		SetFixFrameRate(true);
		break;
	case ECSDebugAutoPilotMode::Random:
		break;
	case ECSDebugAutoPilotMode::Command:
		SetIgnoreDefaultInput(true);
		break;
	default:
		break;
	}
}

/**
 * @brief	���[�h�I����
 */
void UCSDebugAutoPilotComponent::OnEndMode()
{
	switch (mMode)
	{
	case ECSDebugAutoPilotMode::Record:
		SetFixFrameRate(false);
		break;
	case ECSDebugAutoPilotMode::Random:
		break;
	case ECSDebugAutoPilotMode::Command:
		SetIgnoreDefaultInput(false);
		break;
	default:
		break;
	}
}

/**
 * @brief	�t���[�����[�g�Œ�
 */
void UCSDebugAutoPilotComponent::SetFixFrameRate(bool InFix)
{
#if 1//4.21�ŌŒ���@���ς�����H
	if (GEngine)
	{
		GEngine->bUseFixedFrameRate = InFix;
		GEngine->FixedFrameRate = 30.f;
	}
#else
	FApp::SetUseFixedTimeStep(true);
	FApp::SetFixedDeltaTime(0.008888);
#endif
}


#if 0

/**
 * @brief	�����_���C���v�b�g�J�n
 */
void UCSDebugAutoPilotComponent::RequestBeginRandom()
{
	SetMode(ECSDebugAutoPilotMode::Random);
	UAutoPilotModeRandom* ModeRandom = Cast<UAutoPilotModeRandom>(mActiveMode);
	ModeRandom->RequestBegin();
	SetIgnoreDefaultInput(true);

	UDebugCommandManager* DebugCommandManager = GetDebugCommandManager();
	if (DebugCommandManager)
	{
		DebugCommandManager->SetSleep(true);
	}
}

/**
 * @brief	�����_���C���v�b�g�I��
 */
void UCSDebugAutoPilotComponent::RequestEndRandom()
{
	SetMode(ECSDebugAutoPilotMode::Random);
	UAutoPilotModeRandom* ModeRandom = Cast<UAutoPilotModeRandom>(mActiveMode);
	ModeRandom->RequestEnd();
	SetIgnoreDefaultInput(false);

	UDebugCommandManager* DebugCommandManager = GetDebugCommandManager();
	if (DebugCommandManager)
	{
		DebugCommandManager->SetSleep(false);
	}
}

/**
 * @brief	AutoPlay�J�n
 */
void UCSDebugAutoPilotComponent::RequestBeginAutoPlay()
{
	_LOG(LogAI, Log, TEXT("BeginAutoPlay"));

	SetMode(ECSDebugAutoPilotMode::AutoPlay);
	UAutoPilotModeAutoPlay* ModeAutoPlay = Cast<UAutoPilotModeAutoPlay>(mActiveMode);
	ModeAutoPlay->RequestBegin();
	SetIgnoreDefaultInput(true);

	UDebugCommandManager* DebugCommandManager = GetDebugCommandManager();
	if (DebugCommandManager)
	{
		DebugCommandManager->SetSleep(true);
	}
}

/**
 * @brief	AutoPlay�I��
 */
void UCSDebugAutoPilotComponent::RequestEndAutoPlay()
{
	if (UAutoPilotModeAutoPlay* ModeAutoPlay = Cast<UAutoPilotModeAutoPlay>(mActiveMode))
	{
		ModeAutoPlay->RequestEnd();
	}
	SetIgnoreDefaultInput(false);

	SetMode(ECSDebugAutoPilotMode::Invalid);

	UDebugCommandManager* DebugCommandManager = GetDebugCommandManager();
	if (DebugCommandManager)
	{
		DebugCommandManager->SetSleep(false);
	}
}

/**
 * @brief	SemiAutoPlay�J�n
 */
void UCSDebugAutoPilotComponent::RequestBeginSemiAutoPlay()
{
	_LOG(LogAI, Log, TEXT("BeginSemiAutoPlay"));

	SetMode(ECSDebugAutoPilotMode::SemiAutoPlay);
	UAutoPilotModeSemiAutoPlay* ModeSemiAutoPlay = Cast<UAutoPilotModeSemiAutoPlay>(mActiveMode);
	ModeSemiAutoPlay->RequestBegin();
}

/**
 * @brief	SemiAutoPlay�I��
 */
void UCSDebugAutoPilotComponent::RequestEndSemiAutoPlay()
{
	if (UAutoPilotModeSemiAutoPlay* ModeSemiAutoPlay = Cast<UAutoPilotModeSemiAutoPlay>(mActiveMode))
	{
		ModeSemiAutoPlay->RequestEnd();
	}

	SetMode(ECSDebugAutoPilotMode::Invalid);
}

/**
 * @brief	UI���ŃL�[����������
 */
void UCSDebugAutoPilotComponent::OnPushUIKey(const FKey& InKey)
{
	if (mActiveMode)
	{
		mActiveMode->OnPushUIKey(InKey);
	}
}

/**
 * @brief	KeyId
 */
uint32 UCSDebugAutoPilotComponent::GetKeyId(const FKey* InKey) const
{
	const ECSDebugAutoPilotKey* KeyId = mKeyMap.Find(InKey);
	if (KeyId)
	{
		return static_cast<uint32>(*KeyId);
	}
	return 0;
}
uint32 UCSDebugAutoPilotComponent::GetKeyId(const FKey& InKey) const
{
	//mKeyMap�Ń|�C���^�����̂�߂�ׂ��ȋC��
	for (const auto& Element : mKeyMap)
	{
		if (*Element.Key == InKey)
		{
			return (uint32)Element.Value;
		}
	}
	return 0;
}
/**
 * @brief	Key
 */
const FKey* UCSDebugAutoPilotComponent::GetKey(ECSDebugAutoPilotKey InKey) const
{
	if (InKey == ECSDebugAutoPilotKey::Invalid
		|| InKey == ECSDebugAutoPilotKey::Num)
	{
		return nullptr;
	}
	const FKey* const* KeyPtr = mKeyMap.FindKey(InKey);
	if (KeyPtr == nullptr)
	{
		return nullptr;
	}

	return *KeyPtr;
}

/**
 * @brief	�p�b�h���̓R�}���h���N�G�X�g
 */
int32 UCSDebugAutoPilotComponent::RequestCommandInput(ECSDebugAutoPilotKey InKey, float InInputTime, float InAxisValue)
{
	SetMode(ECSDebugAutoPilotMode::Command);
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	return ModeCommand->RequestCommandInput(InKey, InInputTime, InAxisValue);
}

/**
 * @brief	�ړ��R�}���h���N�G�X�g
 */
int32 UCSDebugAutoPilotComponent::RequestCommandMove(const FVector& InGoalPos)
{
	SetMode(ECSDebugAutoPilotMode::Command);
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	return ModeCommand->RequestCommandMove(InGoalPos);
}
/**
 * @brief	�ړ��I��
 */
int32 UCSDebugAutoPilotComponent::RequestCommandMoveStop()
{
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	if (ModeCommand)
	{
		return ModeCommand->RequestCommandMoveStop();
	}
	return 0;
}
/**
 * @brief	�U���R�}���h���N�G�X�g
 */
int32 UCSDebugAutoPilotComponent::RequestCommandAttack()
{
	return 0;
}
/**
 * @brief	�R�}���h�I�����ǂ���
 */
bool UCSDebugAutoPilotComponent::IsFinishCommand(int32 InCommandId)
{
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	if (ModeCommand)
	{
		return ModeCommand->IsFinishCommand(InCommandId);
	}
	return false;
}
/**
 * @brief	�Ō�Ƀ��N�G�X�g�����R�}���h���I�����ǂ���
 */
bool UCSDebugAutoPilotComponent::IsFinishLastCommand()
{
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	if (ModeCommand)
	{
		return ModeCommand->IsFinishLastCommand();
	}
	return false;
}
/**
 * @brief	�S�R�}���h�I�����ǂ���
 */
bool UCSDebugAutoPilotComponent::IsFinishAllCommand()
{
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	if (ModeCommand)
	{
		return !ModeCommand->IsOwnCommand();
	}
	return true;
}
/**
 * @brief	CommandScript���N�G�X�g
 */
bool UCSDebugAutoPilotComponent::RequestCommandScript(FString InPath)
{
	SetMode(ECSDebugAutoPilotMode::Command);
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	ModeCommand->RequestCommandScript(InPath);
	return true;
}
/**
 * @brief	Move���[�h�̈ړ���擾
 */
FVector UCSDebugAutoPilotComponent::GetCommandMoveGoalPos() const
{
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	if (ModeCommand)
	{
		return ModeCommand->GetGoalPos();
	}
	return FVector::ZeroVector;
}

/**
 * @brief	�f�o�b�O�I�����̃f�o�b�O���
 */
void UCSDebugAutoPilotComponent::DebugUpdateSelectInfo(float DeltaTime)
{
	APlayerController* PlayerControler = GetPlayerController();
	ACharacter* Player = Cast<ACharacter>(PlayerControler->GetPawn());
	if (Player == nullptr)
	{
		return;
	}
	if (Player->GetDebugSelectComponent() == nullptr)
	{
		return;
	}
	UDebugSelectComponent& DebugSelectComponent = *Player->GetDebugSelectComponent();

	if (mActiveMode)
	{
		mActiveMode->DebugUpdateSelectInfo(DebugSelectComponent, DeltaTime);
	}
}

/**
 * @brief	�f�o�b�O�I�����̃f�o�b�O�\��
 */
void UCSDebugAutoPilotComponent::DebugDrawSelected(UCanvas* Canvas, const APlayerController* InPlayerController)
{
	if (mActiveMode)
	{
		mActiveMode->DebugDrawSelected(Canvas, InPlayerController);
	}
}
#endif