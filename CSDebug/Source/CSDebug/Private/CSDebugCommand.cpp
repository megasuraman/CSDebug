// Copyright 2020 SensyuGames.
/**
 * @file CSDebugCommand.cpp
 * @brief DebugCommand�̊Ǘ��N���X
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
    }

	const UCSDebugConfig* CSDebugConfig = GetDefault<UCSDebugConfig>();
    //�f�o�b�O�R�}���h���͏����L��
    if (CSDebugConfig->mDebugCommand_ReadyKey.IsPressed(PlayerInput))
	{
        //�f�o�b�O���j���[on/off
        if (CSDebugConfig->mDebugCommand_DebugMenuKey.IsJustPressed(PlayerInput))
        {
            SwicthDebugMenuActive(PlayerController);
            return true;
        }
		//�f�o�b�O�X�g�b�von/off
		else if (CSDebugConfig->mDebugCommand_DebugStopKey.IsJustPressed(PlayerInput))
		{
            mbDebugStopMode = !mbDebugStopMode;
			SetDebugStop(mbDebugStopMode, PlayerController);
			SetStopMotionBlur(mbDebugCameraMode, PlayerController);
			return true;
		}
		//�f�o�b�O�J����on/off
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
 * @brief	�L����PlayerController��T��
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
 * @brief	DebugStep����
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
 * @brief	���ۂ�DebugCamera��ԂɂȂ������̃`�F�b�N��on/off������
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
 * @brief	DebugMenu��on/off�؂�ւ�
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
 * @brief	DebugStop��on/off
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
 * @brief	MotionBlur��~��on/off
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