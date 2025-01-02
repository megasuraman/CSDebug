// Copyright 2020 SensyuGames.
/**
 * @file CSDebugConfig.cpp
 * @brief CSDebug—p‚ÌConfig
 * @author SensyuGames
 * @date 2020/5/26
 */


#include "CSDebugConfig.h"
#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"
#include "DebugMenu/CSDebug_DebugMenuManager.h"


/**
 * @brief UPlayerInput‚ª‰Ÿ‚³‚ê‚Ä‚é‚©‚Ç‚¤‚©
 */
bool	FCSDebugKey::IsPressed(const class UPlayerInput& InInput) const
{
	if (InInput.IsPressed(mKeyboad)
		|| InInput.IsPressed(mPad))
	{
		return true;
	}
	return false;
}

/**
 * @brief UPlayerInput‚ª‰Ÿ‚³‚ê‚½‚©‚Ç‚¤‚©
 */
bool	FCSDebugKey::IsJustPressed(const class UPlayerInput& InInput) const
{
	if (InInput.WasJustPressed(mKeyboad)
		|| InInput.WasJustPressed(mPad))
	{
		return true;
	}
	return false;
}

/**
 * @brief UPlayerInput‚ª—£‚³‚ê‚½‚©‚Ç‚¤‚©
 */
bool FCSDebugKey::IsJustReleased(const class UPlayerInput& InInput) const
{
	if (InInput.WasJustReleased(mKeyboad)
		|| InInput.WasJustReleased(mPad))
	{
		return true;
	}
	return false;
}

UCSDebugConfig::UCSDebugConfig()
{
	mDebugCommand_ReadyKey.mKeyboad = EKeys::LeftAlt;
	mDebugCommand_ReadyKey.mPad = EKeys::Gamepad_Special_Right;

	mDebugCommand_DebugMenuKey.mKeyboad = EKeys::One;
	mDebugCommand_DebugMenuKey.mPad = EKeys::Gamepad_DPad_Up;

	mDebugCommand_DebugStopKey.mKeyboad = EKeys::Two;
	mDebugCommand_DebugStopKey.mPad = EKeys::Gamepad_DPad_Right;

	mDebugCommand_DebugCameraKey.mKeyboad = EKeys::Three;
	mDebugCommand_DebugCameraKey.mPad = EKeys::Gamepad_DPad_Down;

	mDebugSelect_SelectKey.mKeyboad = EKeys::LeftMouseButton;
	mDebugSelect_SelectKey.mPad = EKeys::Gamepad_FaceButton_Right;

	mDebugMenuManagerClass = UCSDebug_DebugMenuManager::StaticClass();

	mDebugMenu_SelectKey.mKeyboad = EKeys::Enter;
	mDebugMenu_SelectKey.mPad = EKeys::Gamepad_FaceButton_Bottom;
	mDebugMenu_CancelKey.mKeyboad = EKeys::BackSpace;
	mDebugMenu_CancelKey.mPad = EKeys::Gamepad_FaceButton_Right;

	mDebugMenu_UpKey.mKeyboad = EKeys::Up;
	mDebugMenu_UpKey.mPad = EKeys::Gamepad_DPad_Up;
	mDebugMenu_DownKey.mKeyboad = EKeys::Down;
	mDebugMenu_DownKey.mPad = EKeys::Gamepad_DPad_Down;
	mDebugMenu_RightKey.mKeyboad = EKeys::Right;
	mDebugMenu_RightKey.mPad = EKeys::Gamepad_DPad_Right;
	mDebugMenu_LeftKey.mKeyboad = EKeys::Left;
	mDebugMenu_LeftKey.mPad = EKeys::Gamepad_DPad_Left;
}