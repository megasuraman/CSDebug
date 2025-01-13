// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_ShortcutCommand.cpp
 * @brief デバッグ機能をパッドやキーから呼び出す機能
 * @author SensyuGames
 * @date 2020/5/25
 */

#include "CSDebug_ShortcutCommand.h"
#include "CSDebug_Subsystem.h"
#include "CSDebug_Config.h"
#include "DebugMenu/CSDebug_DebugMenuManager.h"

#include "Engine/Canvas.h"
#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"
#include "Engine/DebugCameraController.h"

UCSDebug_ShortcutCommand::UCSDebug_ShortcutCommand()
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
void	UCSDebug_ShortcutCommand::Init()
{
	UCSDebug_DebugMenuManager* DebugMenuManager = UCSDebug_DebugMenuManager::sGet(this);

	const FString BaseDebugMenuPath(TEXT("CSDebug/DebugCommand"));
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath, FString(TEXT("DebugStopOnDebugMenu")), false);
}
/**
 * @brief	Tick
 */
bool	UCSDebug_ShortcutCommand::DebugTick(float InDeltaSecond)
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

	UCSDebug_Subsystem* CSDebug = Cast<UCSDebug_Subsystem>(GetOuter());
	UCSDebug_DebugMenuManager* DebugMenuManager = CSDebug->GetDebugMenuManager();
	mbRequestDebugStopOnDebugMenu = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSDebug/DebugCommand/DebugStopOnDebugMenu")));
    if (!DebugMenuManager->IsActive())
	{
		CheckDebugStep(PlayerController, InDeltaSecond);
		CheckDebugCameraMode(PlayerController);
        CheckSecretCommand(PlayerController);
    }

	const UCSDebug_Config* CSDebugConfig = GetDefault<UCSDebug_Config>();
    //デバッグコマンド入力準備有効
    if (CSDebugConfig->mDebugCommand_ReadyKey.IsPressed(*PlayerInput))
	{
        //デバッグメニューon/off
        if (CSDebugConfig->mDebugCommand_DebugMenuKey.IsJustPressed(*PlayerInput))
        {
            SwicthDebugMenuActive(PlayerController);
            return true;
        }
		//デバッグストップon/off
		else if (CSDebugConfig->mDebugCommand_DebugStopKey.IsJustPressed(*PlayerInput))
		{
            mbDebugStopMode = !mbDebugStopMode;
			SetDebugStop(mbDebugStopMode, PlayerController);
			SetStopMotionBlur(mbDebugCameraMode, PlayerController);
			return true;
		}
		//デバッグカメラon/off
		else if (CSDebugConfig->mDebugCommand_DebugCameraKey.IsJustPressed(*PlayerInput))
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
void	UCSDebug_ShortcutCommand::DebugDraw(UCanvas* InCanvas)
{
}

/**
 * @brief	有効なPlayerControllerを探す
 */
APlayerController*	UCSDebug_ShortcutCommand::FindPlayerController() const
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
void	UCSDebug_ShortcutCommand::CheckDebugStep(APlayerController* InPlayerController, float InDeltaSecond)
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
	const UCSDebug_Config* CSDebugConfig = GetDefault<UCSDebug_Config>();

    if (CSDebugConfig->mDebugCommand_DebugStopKey.IsPressed(*PlayerInput))
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
    else if(CSDebugConfig->mDebugCommand_DebugStopKey.IsJustPressed(*PlayerInput))
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
void	UCSDebug_ShortcutCommand::CheckDebugCameraMode(APlayerController* InPlayerController)
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
void UCSDebug_ShortcutCommand::CheckSecretCommand(APlayerController* InPlayerController)
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

	const UCSDebug_Config* CSDebugConfig = GetDefault<UCSDebug_Config>();
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
void	UCSDebug_ShortcutCommand::SwicthDebugMenuActive(APlayerController* InPlayerController)
{
    UCSDebug_Subsystem* CSDebug = Cast<UCSDebug_Subsystem>(GetOuter());
    UCSDebug_DebugMenuManager* DebugMenuManager = CSDebug->GetDebugMenuManager();
    const bool bOldActiveMenu = DebugMenuManager->IsActive();
    // On -> Off
    if (bOldActiveMenu)
    {
        DebugMenuManager->SetActive(false);
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
        DebugMenuManager->SetActive(true);
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
void	UCSDebug_ShortcutCommand::SetDebugStop(bool bInStop, APlayerController* InPlayerController)
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
void	UCSDebug_ShortcutCommand::SetStopMotionBlur(bool bInStop, APlayerController* InPlayerController)
{
    if (mbStopMotionBlur == bInStop)
    {
        return;
    }

    mbStopMotionBlur = bInStop;
    InPlayerController->ConsoleCommand(FString(TEXT("Show MotionBlur")));
}

#endif//USE_CSDEBUG