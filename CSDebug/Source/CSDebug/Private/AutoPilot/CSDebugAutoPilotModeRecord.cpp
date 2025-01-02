// Copyright 2021 SensyuGames.
/**
 * @file CSDebugAutoPilotModeRecord.cpp
 * @brief �������� �p�b�h���͂�ۑ�,�Đ����郂�[�h
 * @author SensyuGames
 * @date 2021/12/28
 */
#include "AutoPilot/CSDebugAutoPilotModeRecord.h"
#include "AutoPilot/CSDebugAutoPilotComponent.h"

#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

/**
 * @brief	PlayerInput�̏����O
 */
void	UCSDebugAutoPilotModeRecord::PreProcessInput(float DeltaTime)
{
 	//ResetDebugDrawPadInfo();
 
 	if (mMode == ECommandMode::PlayInputRecord)
 	{
 		UpdatePlayInputRecord(DeltaTime);
 	}
}

/**
 * @brief	PlayerInput�̏�����
 */
void	UCSDebugAutoPilotModeRecord::PostProcessInput(float DeltaTime)
{
	UpdateInputRecord(DeltaTime);
}

/**
 * @brief	
 */
void	UCSDebugAutoPilotModeRecord::DebugDraw(class UCanvas* InCanvas)
{
	DebugDrawPad(InCanvas);
}

/**
 * @brief	�ۑ���Đ����̃��[�h�ؑ�
 */
void	UCSDebugAutoPilotModeRecord::SetMode(ECommandMode InMode)
{
	if (mMode == InMode)
	{
		return;
	}

	if (mMode == ECommandMode::PlayInputRecord)
	{
		GetParent()->SetIgnoreDefaultInput(false);
	}

	mMode = InMode;
	mPlayFrame = 0;

	if (mMode == ECommandMode::PlayInputRecord)
	{
		GetParent()->SetIgnoreDefaultInput(true);
	}
}

/**
 * @brief	�Đ�
 */
void UCSDebugAutoPilotModeRecord::RequestPlayInputRecord(const FString& InFileName)
{
	SetMode(ECommandMode::PlayInputRecord);
	mFileName = InFileName;
	mPlayFrame = 0;
	mPlayInputRecordState = EPlayInputRecordState::Load;
}
/**
 * @brief	
 */
bool	UCSDebugAutoPilotModeRecord::IsFinihPlay() const
{
	if (mMode == ECommandMode::PlayInputRecord
		&& mPlayInputRecordState == EPlayInputRecordState::Finish
		)
	{
		return true;
	}
	return false;
}
/**
 * @brief	�R�}���h���^�J�n���N�G�X�g
 */
void UCSDebugAutoPilotModeRecord::RequestBeginRecord(const FString& InFileName)
{
	SetMode(ECommandMode::BeginRecord);
	mFileName = InFileName;
	mPlayFrame = 0;
}
/**
 * @brief	�R�}���h���^�I�����N�G�X�g
 */
void UCSDebugAutoPilotModeRecord::RequestEndRecord()
{
	SetMode(ECommandMode::EndRecord);
}
/**
 * @brief	��~���N�G�X�g
 */
void UCSDebugAutoPilotModeRecord::RequestIdle()
{
	SetMode(ECommandMode::Idle);
}

/**
 * @brief	�o�̓t�@�C���p�X�擾
 */
FString	UCSDebugAutoPilotModeRecord::GetFilePath() const
{
	FString SavedPath = FPaths::ProjectSavedDir();
	SavedPath += "CSDebug/AutoPilot/" + mFileName + ".json";
	return SavedPath;
}

/**
 * @brief	�L�^�������͂��Đ�����
 */
bool UCSDebugAutoPilotModeRecord::UpdatePlayInputRecord(float DeltaTime)
{
	APlayerController* PlayerControler = GetPlayerController();
	//PlayerControler->FlushPressedKeys();
	PlayerControler->InputAxis(EKeys::Gamepad_LeftX, 0.f, DeltaTime, 1, true);
	PlayerControler->InputAxis(EKeys::Gamepad_LeftY, 0.f, DeltaTime, 1, true);
	PlayerControler->InputAxis(EKeys::Gamepad_RightX, 0.f, DeltaTime, 1, true);
	PlayerControler->InputAxis(EKeys::Gamepad_RightY, 0.f, DeltaTime, 1, true);

	switch (mPlayInputRecordState)
	{
	case EPlayInputRecordState::Load:
		if (LoadInputRecordFile(DeltaTime))
		{
			mPlayInputRecordState = EPlayInputRecordState::WaitReady;
		}
		break;
	case EPlayInputRecordState::WaitReady:
		if (!WaitPlayInputRecordFile(DeltaTime))
		{
			mPlayInputRecordState = EPlayInputRecordState::Play;
		}
		break;
	case EPlayInputRecordState::Play:
		if (!PlayInputRecordFile(DeltaTime))
		{
			mPlayInputRecordState = EPlayInputRecordState::Finish;
		}
		break;
	case EPlayInputRecordState::Finish:
		mPlayFrame = 0;
		mWarpInterval = 1.f;
		break;
	default:
		break;
	}

	return true;
}

/**
 * @brief	�L�^�������͂����[�h
 */
bool UCSDebugAutoPilotModeRecord::LoadInputRecordFile(float DeltaTime)
{
	mPlayFrame = 0;
	mCommand.mList.Empty();
	mWarpInterval = 1.f;

	const FString SavedPath = GetFilePath();
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *SavedPath))
	{
		return false;
	}

	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject)
		&& JsonObject.IsValid()
		)
	{
		mCommand.FromJson(JsonObject);
	}
	else
	{
		SetMode(ECommandMode::Invalid);
		return false;
	}

	if (mCommand.mList.Num() == 0)
	{
		SetMode(ECommandMode::Invalid);
		return false;
	}

	return true;
}

/**
 * @brief	�L�^�������͂��Đ�����܂ł̑҂�
 */
bool UCSDebugAutoPilotModeRecord::WaitPlayInputRecordFile(float DeltaTime)
{
	APlayerController* PlayerControler = GetPlayerController();
	ACharacter* Player = Cast<ACharacter>(PlayerControler->GetPawn());
	if (Player)
	{
		const FVector StartPos(mCommand.mStartPosX, mCommand.mStartPosY, mCommand.mStartPosZ);
		const FRotator StartRot(mCommand.mStartRotatorPitch, mCommand.mStartRotatorYaw, mCommand.mStartRotatorRoll);
		const FRotator CameraRot(mCommand.mStartCameraRotatorPitch, mCommand.mStartCameraRotatorYaw, mCommand.mStartCameraRotatorRoll);
		const FRotator ControlRot(mCommand.mStartControllerPitch, StartRot.Yaw, CameraRot.Yaw);
		if (FVector::DistSquared(StartPos, Player->GetActorLocation()) > FMath::Square(0.1f)
			|| !StartRot.Equals(Player->GetActorRotation()))
		{
			Player->SetActorLocationAndRotation(StartPos, StartRot, false, nullptr, ETeleportType::TeleportPhysics);
			//PlayerControler->SetControlRotation(CameraRot, ControlRot);
			return true;
		}
	}

	if (mWarpInterval > 0.f)
	{
		mWarpInterval -= DeltaTime;
		return true;
	}

	return false;
}

/**
 * @brief	�L�^�������͂����ۂɍĐ�
 */
bool UCSDebugAutoPilotModeRecord::PlayInputRecordFile(float DeltaTime)
{
	UCSDebugAutoPilotComponent* Parent = GetParent();
	APlayerController* PlayerControler = GetPlayerController();

	for (const FCommandNode& InCommand : mCommand.mList)
	{
		const ECSDebugAutoPilotKey KeyId = static_cast<ECSDebugAutoPilotKey>(InCommand.mKeyId);
		const FKey Key = GetKey(KeyId);
		if (mPlayFrame >= InCommand.mBeginFrame
			&& mPlayFrame <= InCommand.mEndFrame)
		{
			if (Key.IsAxis1D())
			//if (Key.IsFloatAxis())
			{
				PlayerControler->InputAxis(Key, InCommand.mAxisValue, InCommand.mDeltaTime, 1, true);
				AddDebugDrawPadInfo(FCSDebugAutoPilotDebugDrawPadInfo(KeyId, InCommand.mAxisValue));
			}
			else
			{
				if (mPlayFrame == InCommand.mBeginFrame)
				{
					PlayerControler->InputKey(Key, EInputEvent::IE_Pressed, 1.f, true);
				}
				else
				{
					PlayerControler->InputKey(Key, EInputEvent::IE_Repeat, 1.f, true);
				}
				AddDebugDrawPadInfo(FCSDebugAutoPilotDebugDrawPadInfo(KeyId, 1.f));
			}
		}
		else if (InCommand.mEndFrame + 1 == mPlayFrame)
		{//�O�t���[���ŏ������Ă�
			if (Key.IsAxis1D())
			//if (Key.IsFloatAxis())
			{
				//PlayerControler->InputAxis(*Key, InCommand.mAxisValue, InCommand.mDeltaTime, 1, true);
			}
			else
			{
				PlayerControler->InputKey(Key, EInputEvent::IE_Released, 1.f, true);
			}
		}
	}

	mPlayFrame += 1;

	return (mPlayFrame <= mCommand.mEndFrame);
}

/**
 * @brief	���͂̋L�^
 */
bool UCSDebugAutoPilotModeRecord::UpdateInputRecord(float DeltaTime)
{
	switch (mMode)
	{
	case ECommandMode::BeginRecord:
		BeginInputRecord(DeltaTime);
		break;
	case ECommandMode::Record:
		RecordingInput(DeltaTime);
		break;
	case ECommandMode::EndRecord:
	{
		const FString SavedPath = GetFilePath();
		const bool bSave = FFileHelper::SaveStringToFile(mCommand.ToJson(), *SavedPath);
		SetMode(ECommandMode::Invalid);
		break;
	}
	default:
		break;
	}
	return true;
}
/**
 * @brief	���͂̋L�^���J�n
 */
bool UCSDebugAutoPilotModeRecord::BeginInputRecord(float DeltaTime)
{
	APlayerController* PlayerControler = GetPlayerController();
	mCommand.mList.Empty();
	mBeforeFrameCommandList.Empty();
	ACharacter* Player = Cast<ACharacter>(PlayerControler->GetPawn());
	if (Player)
	{
		mCommand.mStartPosX = Player->GetActorLocation().X;
		mCommand.mStartPosY = Player->GetActorLocation().Y;
		mCommand.mStartPosZ = Player->GetActorLocation().Z;
		mCommand.mStartRotatorPitch = Player->GetActorRotation().Pitch;
		mCommand.mStartRotatorYaw = Player->GetActorRotation().Yaw;
		mCommand.mStartRotatorRoll = Player->GetActorRotation().Roll;

		const FRotator CameraRot = PlayerControler->GetControlRotation();
		mCommand.mStartCameraRotatorPitch = CameraRot.Pitch;
		mCommand.mStartCameraRotatorYaw = CameraRot.Yaw;
		mCommand.mStartCameraRotatorRoll = CameraRot.Roll;
		mCommand.mStartControllerPitch = PlayerControler->GetControlRotation().Pitch;
	}

	mPlayFrame = 0;
	SetMode(ECommandMode::Record);

	return true;
}
/**
 * @brief	���͂̋L�^
 */
bool UCSDebugAutoPilotModeRecord::RecordingInput(float DeltaTime)
{
	UCSDebugAutoPilotComponent* Parent = GetParent();
	APlayerController* PlayerControler = GetPlayerController();
	CommandPtrList ActiveCommandPtrList;

	bool bAnyInput = false;
	for (uint32 i = 0; i < static_cast<uint32>(ECSDebugAutoPilotKey::Num); ++i)
	{
		const ECSDebugAutoPilotKey KeyId = static_cast<ECSDebugAutoPilotKey>(i);
		const FKey Key = GetKey(KeyId);
		FCommandNode Temp;
		Temp.mBeginFrame = mPlayFrame;
		Temp.mEndFrame = mPlayFrame;
		Temp.mDeltaTime = DeltaTime;
		Temp.mKeyId = i;
		bool bInput = false;
		if (Key.IsAxis1D())
		//if (Key.IsFloatAxis())
		{
			Temp.mAxisValue = PlayerControler->GetInputAnalogKeyState(Key);
			const float* PadDeadZonePtr = GetPadDeadZoneMap().Find(Key);
			if (PadDeadZonePtr == nullptr)
			{
				continue;
			}
			const float PadDeadZone = *PadDeadZonePtr;
			if (Temp.mAxisValue > 0.f)
			{
				Temp.mAxisValue = Temp.mAxisValue * (1.0f - PadDeadZone) + PadDeadZone;
			}
			else if (Temp.mAxisValue < 0.f)
			{
				Temp.mAxisValue = Temp.mAxisValue * (1.0f - PadDeadZone) - PadDeadZone;
			}
			if (FMath::Abs(Temp.mAxisValue) > PadDeadZone)
			{
				bInput = true;
				AddDebugDrawPadInfo(FCSDebugAutoPilotDebugDrawPadInfo(KeyId, Temp.mAxisValue));
			}
		}
		else
		{
			if (PlayerControler->IsInputKeyDown(Key))
			{
				bInput = true;
				Temp.mInputEventId = EInputEvent::IE_Pressed;
				AddDebugDrawPadInfo(FCSDebugAutoPilotDebugDrawPadInfo(KeyId, 1.f));
			}
		}

		if (bInput)
		{
			bAnyInput = true;

			int32 InputIndex = INDEX_NONE;
			for (int32 f = 0; f < mBeforeFrameCommandList.Num(); ++f)
			{
				if (mBeforeFrameCommandList[f]->IsSameInput(Temp))
				{
					InputIndex = mBeforeFrameCommandList[f]->mIndex;
					break;
				}
			}
			if (InputIndex == INDEX_NONE)
			{
				mCommand.mList.Add(Temp);
				InputIndex = mCommand.mList.Num() - 1;
			}
			FCommandNode* InputCommand = &mCommand.mList[InputIndex];
			InputCommand->mEndFrame = mPlayFrame;
			InputCommand->mIndex = InputIndex;
			ActiveCommandPtrList.Add(InputCommand);
		}
	}

	++mPlayFrame;

	mBeforeFrameCommandList = ActiveCommandPtrList;

	mCommand.mEndFrame = mPlayFrame;

	//���̂�Widget�̐؂�ւ���PlayerInput���X�V����Ȃ��Ȃ�̂�
	//if (bInputUI
	//	&& bAnyInput)
	//{
	//	PlayerControler->PlayerInput->FlushPressedKeys();
	//}

	return true;
}

#if 0
/**
 * @brief	�f�o�b�O�I�����̃f�o�b�O���
 */
void	UCSDebugAutoPilotModeRecord::DebugUpdateSelectInfo(UDebugSelectComponent& InDebugSelect, float DeltaTime)
{
	InDebugSelect.AddDebugInfo(FString::Printf(TEXT("mPlayFrame : %d"), mPlayFrame));
	InDebugSelect.AddDebugInfo(FString::Printf(TEXT("DeltaTime : %.5f"), DeltaTime));

	APlayerController* PlayerControler = GetPlayerController();
	APawn* Player = PlayerControler->GetPawn();
	InDebugSelect.AddDebugInfo(FString::Printf(TEXT("Pos : %s"), *Player->GetActorLocation().ToString()));

	DebugUpdateSelectInfo_Player(InDebugSelect);
	if (mMode == ECommandMode::PlayInputRecord
		|| mMode == ECommandMode::Record)
	{
		DebugUpdateSelectInfo_PlayKey(InDebugSelect);
	}
	else
	{
		DebugUpdateSelectInfo_InputKey(InDebugSelect);
	}
}
/**
 * @brief	�f�o�b�O�I�����̃f�o�b�O�\��
 */
void	UCSDebugAutoPilotModeRecord::DebugDrawSelected(UCanvas* Canvas, const APlayerController* InPlayerController)
{
	DebugDrawPad(Canvas, InPlayerController);
}

/**
 * @brief	UI���ŃL�[����������
 *			RecordingInput()���Ɖ��̂������widget�̓���̃{�^���ł���PlayerInput���������Ȃ��̂Ő�p�ɗp��
 */
void	UCSDebugAutoPilotModeRecord::OnPushUIKey(const FKey& InKey)
{
	if (mMode != ECommandMode::Record
		|| !IsInputUI())
	{
		return;
	}
	
	UCSDebugAutoPilotComponent* Parent = GetParent();
	FCommandNode Temp;
	Temp.mBeginFrame = mPlayFrame;
	Temp.mEndFrame = mPlayFrame + 5;
	Temp.mAxisValue = 1.f;
	Temp.mDeltaTime = 0.03f;
	Temp.mKeyId = Parent->GetKeyId(InKey);
	Temp.mInputEventId = EInputEvent::IE_Pressed;

	mCommand.mList.Add(Temp);
	const int32 InputIndex = mCommand.mList.Num() - 1;
	FCommandNode* InputCommand = &mCommand.mList[InputIndex];
	InputCommand->mIndex = InputIndex;

	AddDebugDrawPadInfo(FCSDebugAutoPilotDebugDrawPadInfo((ECSDebugAutoPilotMode)Temp.mKeyId, 1.f));

	_LOG(LogSystem, Log, TEXT("OnPushUIKey %d %.2f"), Temp.mKeyId, GetParent()->GetWorld()->GetTimeSeconds());
}


/**
 * @brief	Player���\��
 */
void	UCSDebugAutoPilotModeRecord::DebugUpdateSelectInfo_Player(UDebugSelectComponent& InDebugSelect)
{
	APlayerController* PlayerControler = GetPlayerController();
	APlayer* Player = Cast<APlayer>(PlayerControler->GetPawn());
	if (Player == nullptr)
	{
		return;
	}

	InDebugSelect.BeginDebugInfoCategory(FString::Printf(TEXT("PlayerState")));
	{
		const UActionStateComponent_Player* ActionStateComponent = Player->GetActionState();
		InDebugSelect.AddDebugInfo(FString::Printf(TEXT("PlayerActionState : %s"), *PlayerStateToString(ActionStateComponent->GetCurrentPlayerStateType()).ToString()));
		InDebugSelect.AddDebugInfo(FString::Printf(TEXT("Guts : %d"), Player->GetEnableWilson()));
		//DebugSelectComponent->AddDebugInfo(FString::Printf(TEXT("PlayerMoveType : %s"), *DebugGetDispNamePlayerMoveType()));
		//const UStatusComponent_Player* StatusComponent = Player->GetStatusComponent();
	}
	InDebugSelect.EndDebugInfoCategory();
}
/**
 * @brief	���͂��Ă�Key���\��
 */
void	UCSDebugAutoPilotModeRecord::DebugUpdateSelectInfo_InputKey(UDebugSelectComponent& InDebugSelect)
{
	UCSDebugAutoPilotComponent* Parent = GetParent();
	APlayerController* PlayerControler = GetPlayerController();
	InDebugSelect.BeginDebugInfoCategory(FString::Printf(TEXT("Input")));
	{
		for (uint32 i = 0; i < static_cast<uint32>(ECSDebugAutoPilotMode::Num); ++i)
		{
			const FKey* Key = Parent->GetKey(static_cast<ECSDebugAutoPilotMode>(i));
			if (Key == nullptr)
			{
				continue;
			}
			//if (Key.IsAxis1D())
			if (Key.IsFloatAxis())
			{
				const float AxisValue = PlayerControler->GetInputAnalogKeyState(*Key);
				if (FMath::Abs(AxisValue) > 0.f)
				{
					InDebugSelect.AddDebugInfo(FString::Printf(TEXT("Key : %s(%.2f)"), *AutoPilotUtil::GetAutoPilotKeyName(static_cast<ECSDebugAutoPilotMode>(i)), AxisValue));
				}
			}
			else
			{
				if (PlayerControler->IsInputKeyDown(*Key))
				{
					InDebugSelect.AddDebugInfo(FString::Printf(TEXT("Key : %s"), *AutoPilotUtil::GetAutoPilotKeyName(static_cast<ECSDebugAutoPilotMode>(i))));
				}
			}
		}
	}
	InDebugSelect.EndDebugInfoCategory();
}
/**
 * @brief	�Đ�����Key���\��
 */
void	UCSDebugAutoPilotModeRecord::DebugUpdateSelectInfo_PlayKey(UDebugSelectComponent& InDebugSelect)
{
	UCSDebugAutoPilotComponent* Parent = GetParent();
	InDebugSelect.BeginDebugInfoCategory(FString::Printf(TEXT("Play(%d)"), (uint8)mPlayInputRecordState));

	uint32 PlayFrame = mPlayFrame;
	if (mMode == ECommandMode::Record
		&& mPlayFrame > 0)
	{
		--PlayFrame;
	}

	for (const FCommandNode& InCommand : mCommand.mList)
	{
		if (PlayFrame >= InCommand.mBeginFrame
			&& PlayFrame <= InCommand.mEndFrame)
		{
			const ECSDebugAutoPilotMode AutoPilotKey = static_cast<ECSDebugAutoPilotMode>(InCommand.mKeyId);
			InDebugSelect.BeginDebugInfoCategory(FString::Printf(TEXT("Key : %s"), *AutoPilotUtil::GetAutoPilotKeyName(AutoPilotKey)));
			{
				InDebugSelect.AddDebugInfo(FString::Printf(TEXT("Frame : %d - %d"), InCommand.mBeginFrame, InCommand.mEndFrame));
				InDebugSelect.AddDebugInfo(FString::Printf(TEXT("DeltaTime : %.2f"), InCommand.mDeltaTime));
				const FKey* Key = Parent->GetKey(AutoPilotKey);
				if (Key)
				{
					//if (Key.IsAxis1D())
					if (Key.IsFloatAxis())
					{
						InDebugSelect.AddDebugInfo(FString::Printf(TEXT("AxisValue : %.2f"), InCommand.mAxisValue));
					}
					else
					{
						InDebugSelect.AddDebugInfo(FString::Printf(TEXT("InputEvent : IE_Pressed")));
					}
				}
			}
			InDebugSelect.EndDebugInfoCategory();
		}
	}

	InDebugSelect.EndDebugInfoCategory();
}
#endif