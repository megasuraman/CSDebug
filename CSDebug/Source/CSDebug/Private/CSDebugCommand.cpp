// Copyright 2020 SensyuGames.
/**
 * @file CSDebugCommand.cpp
 * @brief DebugCommandの管理クラス
 * @author SensyuGames
 * @date 2020/5/25
 */

#include "CSDebugCommand.h"
#include "CSDebugSubsystem.h"
#include "CSDebugConfig.h"
#include "DebugMenu/CSDebugMenuManager.h"
#include "DebugMenu/CSDebugMenuNodeGetter.h"

#include "Engine/Canvas.h"
#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"
#include "Engine/DebugCameraController.h"

UCSDebugCommand::UCSDebugCommand()
#if USE_CSDEBUG
    :mbDebugStopMode(false)
	, mbDebugCameraMode(false)
	, mbDebugStop(false)
	, mbDebugStep(false)
	, mbDebugStepRepeat(false)
	, mbStopMotionBlur(false)
#endif//USE_CSDEBUG
{}

#if USE_CSDEBUG
/**
 * @brief	Init
 */
void	UCSDebugCommand::Init()
{
	UCSDebugMenuManager* CSDebugMenu = UCSDebugMenuManager::Get(this);

	const FString BaseDebugMenuPath(TEXT("CSDebug/DebugCommand/"));
	CSDebugMenu->AddNodePropertyBool(BaseDebugMenuPath + TEXT("DebugStopOnDebugMenu"), mbRequestDebugStopOnDebugMenu);
}
/**
 * @brief	Tick
 */
bool	UCSDebugCommand::DebugTick(float InDeltaSecond)
{
    APlayerController* PlayerController = FindPlayerController();
    if (PlayerController == nullptr)
    {
        return true;
    }

	const UPlayerInput* PlayerInput = PlayerController->PlayerInput;
	if (PlayerInput == nullptr)
	{
		return true;
	}

	UCSDebugSubsystem* CSDebug = Cast<UCSDebugSubsystem>(GetOuter());
	UCSDebugMenuManager* DebugMenu = CSDebug->GetDebugMenuManager();
    const bool bActiveDebugMenu = DebugMenu->IsActive();
    if (!bActiveDebugMenu)
	{
		CheckDebugStep(PlayerController, InDeltaSecond);
		CheckDebugCameraMode(PlayerController);
        CheckSecretCommand(PlayerController);
    }

	const UCSDebugConfig* CSDebugConfig = GetDefault<UCSDebugConfig>();
    //デバッグコマンド入力準備有効
    if (CSDebugConfig->mDebugCommand_ReadyKey.IsPressed(PlayerInput))
	{
        //デバッグメニューon/off
        if (CSDebugConfig->mDebugCommand_DebugMenuKey.IsJustPressed(PlayerInput))
        {
            SwicthDebugMenuActive(PlayerController);
            return true;
        }
		//デバッグストップon/off
		else if (CSDebugConfig->mDebugCommand_DebugStopKey.IsJustPressed(PlayerInput))
		{
            mbDebugStopMode = !mbDebugStopMode;
			SetDebugStop(mbDebugStopMode, PlayerController);
			SetStopMotionBlur(mbDebugCameraMode, PlayerController);
			return true;
		}
		//デバッグカメラon/off
		else if (CSDebugConfig->mDebugCommand_DebugCameraKey.IsJustPressed(PlayerInput))
		{
			PlayerController->ConsoleCommand(FString(TEXT("ToggleDebugCamera")));
			return true;
		}
    }

    return true;
}

/**
 * @brief	Draw
 */
void	UCSDebugCommand::DebugDraw(UCanvas* InCanvas)
{
}

/**
 * @brief	有効なPlayerControllerを探す
 */
APlayerController*	UCSDebugCommand::FindPlayerController() const
{
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        if (APlayerController* PlayerController = Iterator->Get())
        {
            if (PlayerController->Player)
            {
                return PlayerController;
            }
        }
    }
    return nullptr;
}

/**
 * @brief	DebugStep処理
 */
void	UCSDebugCommand::CheckDebugStep(APlayerController* InPlayerController, float InDeltaSecond)
{
    if (!mbDebugStopMode)
    {
        return;
    }

    if (mbDebugStep)
    {
        if (!mbDebugStop)
		{
			mDebugStepInterval -= InDeltaSecond;
			if (mDebugStepInterval < 0.f)
			{
                SetDebugStop(true, InPlayerController);
                mbDebugStep = false;
			}
        }
    }

	UPlayerInput* PlayerInput = InPlayerController->PlayerInput;
	const UCSDebugConfig* CSDebugConfig = GetDefault<UCSDebugConfig>();

    if (CSDebugConfig->mDebugCommand_DebugStopKey.IsPressed(PlayerInput))
    {
        if (!mbDebugStepRepeat)
		{
			mDebugStepRepeatBeginTimer += InDeltaSecond;
            if (mDebugStepRepeatBeginTimer > mDebugStepRepeatBeginSec)
            {
                mbDebugStepRepeat = true;
                mDebugStepRepeatStopTimer = 0.f;
            }
        }
    }
    else
    {
        mbDebugStepRepeat = false;
    }

    if (mbDebugStepRepeat)
	{
		if (mbDebugStop)
		{
            mDebugStepRepeatStopTimer += InDeltaSecond;
            if (mDebugStepRepeatStopTimer > mDebugStepRepeatStopSec)
			{
                mDebugStepRepeatStopTimer = 0.f;
				mbDebugStep = true;
				mDebugStepInterval = 0.f;
				SetDebugStop(false, InPlayerController);
            }
		}
    }
    else if(CSDebugConfig->mDebugCommand_DebugStopKey.IsJustPressed(PlayerInput))
    {
        if (mbDebugStop)
		{
			mbDebugStep = true;
			mDebugStepInterval = 0.f;
			SetDebugStop(false, InPlayerController);
        }
    }
}

/**
 * @brief	実際にDebugCamera状態になったかのチェックとon/off時処理
 */
void	UCSDebugCommand::CheckDebugCameraMode(APlayerController* InPlayerController)
{
    ADebugCameraController* DebugCameraController = Cast<ADebugCameraController>(InPlayerController);
    const bool bDebugCamera = (DebugCameraController != nullptr);
    if (mbDebugCameraMode == bDebugCamera)
    {
        return;
    }

    mbDebugCameraMode = bDebugCamera;
    if (mbDebugCameraMode)
	{
        if (DebugCameraController->IsDisplayEnabled())
        {
            DebugCameraController->ToggleDisplay();
		}
		SetStopMotionBlur(mbDebugStopMode, InPlayerController);
    }
    else
	{
		SetStopMotionBlur(false, InPlayerController);
    }
}

/**
 * @brief	SecretCommandチェック
 */
void UCSDebugCommand::CheckSecretCommand(APlayerController* InPlayerController)
{
	UPlayerInput* PlayerInput = InPlayerController->PlayerInput;
    const float NowTime = GetWorld()->TimeSeconds;
    const bool bInputAnyKey = PlayerInput->WasJustPressed(EKeys::Gamepad_FaceButton_Bottom)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_FaceButton_Right)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_FaceButton_Left)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_FaceButton_Top)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_LeftShoulder)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_RightShoulder)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_LeftTrigger)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_RightTrigger)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_DPad_Up)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_DPad_Down)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_DPad_Right)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_DPad_Left);

	const UCSDebugConfig* CSDebugConfig = GetDefault<UCSDebugConfig>();
    const TMap<FString, FCSDebugSecretCommand>& SecretCommand = CSDebugConfig->mDebugSecretCommand;
    for (const auto& Element : SecretCommand)
    {
		bool bPossibleToRunCommand = false;
		bool bNeedDeleteLog = false;
        const TArray<FKey>& KeyList = Element.Value.mKeyList;
        if (FSecretCommandLog* SecretCommandLog = mSecretCommandLog.Find(Element.Key))
		{
			if (PlayerInput->WasJustPressed(KeyList[SecretCommandLog->mNextIndex]))
			{
                SecretCommandLog->mNextIndex += 1;
                SecretCommandLog->mInputTime = NowTime;
				if (SecretCommandLog->mNextIndex >= KeyList.Num())
				{
					bPossibleToRunCommand = true;
				}
            }
            else if(bInputAnyKey
                    || NowTime - SecretCommandLog->mInputTime > 3.f )
            {
                bNeedDeleteLog = true;
            }
        }
        else if (KeyList.Num() > 0)
        {
            if (PlayerInput->WasJustPressed(KeyList[0]))
            {
                FSecretCommandLog& NewSecretCommandLog = mSecretCommandLog.FindOrAdd(Element.Key);
                NewSecretCommandLog.mNextIndex = 1;
                NewSecretCommandLog.mInputTime = GetWorld()->TimeSeconds;
                if (NewSecretCommandLog.mNextIndex >= KeyList.Num())
                {
                    bPossibleToRunCommand = true;
                }
            }
        }

        if (bPossibleToRunCommand)
		{
			InPlayerController->ConsoleCommand(Element.Key);
            bNeedDeleteLog = true;
        }

        if (bNeedDeleteLog)
        {
            mSecretCommandLog.Remove(Element.Key);
        }
    }
}

/**
 * @brief	DebugMenuのon/off切り替え
 */
void	UCSDebugCommand::SwicthDebugMenuActive(APlayerController* InPlayerController)
{
    UCSDebugSubsystem* CSDebug = Cast<UCSDebugSubsystem>(GetOuter());
    UCSDebugMenuManager* DebugMenu = CSDebug->GetDebugMenuManager();
    const bool bOldActiveMenu = DebugMenu->IsActive();
    // On -> Off
    if (bOldActiveMenu)
    {
        DebugMenu->SetActive(false);
        if (mbRequestReleaseDebugStopAfterMenu)
        {
            SetDebugStop(false, InPlayerController);
            SetStopMotionBlur(false, InPlayerController);
        }
        mbRequestReleaseDebugStopAfterMenu = false;
    }
    // Off -> On
    else
    {
        DebugMenu->SetActive(true);
        if (mbRequestDebugStopOnDebugMenu
            && !mbDebugStopMode)
        {
            mbRequestReleaseDebugStopAfterMenu = true;
            SetDebugStop(true, InPlayerController);
            SetStopMotionBlur(true, InPlayerController);
        }
    }
}

/**
 * @brief	DebugStopのon/off
 */
void	UCSDebugCommand::SetDebugStop(bool bInStop, APlayerController* InPlayerController)
{
    if (mbDebugStop == bInStop)
    {
        return;
    }

	mbDebugStop = bInStop;
    InPlayerController->ConsoleCommand(FString(TEXT("Pause")));
    if (mbDebugStop)
    {
        mDebugStepRepeatBeginTimer = 0.f;
    }
    else
    {

    }
}

/**
 * @brief	MotionBlur停止のon/off
 */
void	UCSDebugCommand::SetStopMotionBlur(bool bInStop, APlayerController* InPlayerController)
{
    if (mbStopMotionBlur == bInStop)
    {
        return;
    }

    mbStopMotionBlur = bInStop;
    InPlayerController->ConsoleCommand(FString(TEXT("Show MotionBlur")));
}

#endif//USE_CSDEBUG