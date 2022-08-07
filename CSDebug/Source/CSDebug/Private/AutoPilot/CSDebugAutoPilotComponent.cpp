// Copyright 2020 SensyuGames.
/**
 * @file CSDebugAutoPilotComponent.cpp
 * @brief PlayerControllerに対して自動入力等を補助するComponent
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
 * @brief PlayerInputの処理前
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
 * @brief PlayerInputの処理後
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
 * @brief APlayerControllerを取得
 */
APlayerController* UCSDebugAutoPilotComponent::GetPlayerController()
{
	return Cast<APlayerController>(GetOwner());
}

/**
 * @brief Modeセット
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
 * @brief 通常の入力処理の無効化
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
   !再生
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
 * @brief 再生終わったかどうか
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
 * @brief コマンド収録開始
 */
void UCSDebugAutoPilotComponent::RequestBeginRecord(const FString& InFileName)
{
	SetMode(ECSDebugAutoPilotMode::Record);
	UCSDebugAutoPilotModeRecord* ModeRecord = Cast<UCSDebugAutoPilotModeRecord>(mActiveMode);
	ModeRecord->RequestBeginRecord(InFileName);
}

/**
 * @brief コマンド収録終了
 */
void UCSDebugAutoPilotComponent::RequestEndRecord()
{
	SetMode(ECSDebugAutoPilotMode::Record);
	UCSDebugAutoPilotModeRecord* ModeRecord = Cast<UCSDebugAutoPilotModeRecord>(mActiveMode);
	ModeRecord->RequestEndRecord();
}

/**
 * @brief コマンド収録のIdle状態
 */
void UCSDebugAutoPilotComponent::RequestIdleRecord()
{
	SetMode(ECSDebugAutoPilotMode::Record);
	UCSDebugAutoPilotModeRecord* ModeRecord = Cast<UCSDebugAutoPilotModeRecord>(mActiveMode);
	ModeRecord->RequestIdle();
}

/**
 * @brief	Draw登録のon/off
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
 * @brief	モード開始時
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
 * @brief	モード終了時
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
 * @brief	フレームレート固定
 */
void UCSDebugAutoPilotComponent::SetFixFrameRate(bool InFix)
{
#if 1//4.21で固定方法が変わった？
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
 * @brief	ランダムインプット開始
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
 * @brief	ランダムインプット終了
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
 * @brief	AutoPlay開始
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
 * @brief	AutoPlay終了
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
 * @brief	SemiAutoPlay開始
 */
void UCSDebugAutoPilotComponent::RequestBeginSemiAutoPlay()
{
	_LOG(LogAI, Log, TEXT("BeginSemiAutoPlay"));

	SetMode(ECSDebugAutoPilotMode::SemiAutoPlay);
	UAutoPilotModeSemiAutoPlay* ModeSemiAutoPlay = Cast<UAutoPilotModeSemiAutoPlay>(mActiveMode);
	ModeSemiAutoPlay->RequestBegin();
}

/**
 * @brief	SemiAutoPlay終了
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
 * @brief	UI側でキーを押した時
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
	//mKeyMapでポインタ扱うのやめるべきな気も
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
 * @brief	パッド入力コマンドリクエスト
 */
int32 UCSDebugAutoPilotComponent::RequestCommandInput(ECSDebugAutoPilotKey InKey, float InInputTime, float InAxisValue)
{
	SetMode(ECSDebugAutoPilotMode::Command);
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	return ModeCommand->RequestCommandInput(InKey, InInputTime, InAxisValue);
}

/**
 * @brief	移動コマンドリクエスト
 */
int32 UCSDebugAutoPilotComponent::RequestCommandMove(const FVector& InGoalPos)
{
	SetMode(ECSDebugAutoPilotMode::Command);
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	return ModeCommand->RequestCommandMove(InGoalPos);
}
/**
 * @brief	移動終了
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
 * @brief	攻撃コマンドリクエスト
 */
int32 UCSDebugAutoPilotComponent::RequestCommandAttack()
{
	return 0;
}
/**
 * @brief	コマンド終了かどうか
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
 * @brief	最後にリクエストしたコマンドが終了かどうか
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
 * @brief	全コマンド終了かどうか
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
 * @brief	CommandScriptリクエスト
 */
bool UCSDebugAutoPilotComponent::RequestCommandScript(FString InPath)
{
	SetMode(ECSDebugAutoPilotMode::Command);
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	ModeCommand->RequestCommandScript(InPath);
	return true;
}
/**
 * @brief	Moveモードの移動先取得
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
 * @brief	デバッグ選択時のデバッグ情報
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
 * @brief	デバッグ選択時のデバッグ表示
 */
void UCSDebugAutoPilotComponent::DebugDrawSelected(UCanvas* Canvas, const APlayerController* InPlayerController)
{
	if (mActiveMode)
	{
		mActiveMode->DebugDrawSelected(Canvas, InPlayerController);
	}
}
#endif