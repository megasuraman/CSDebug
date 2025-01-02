// Copyright 2021 SensyuGames.
/**
 * @file CSDebugAutoPilotModeBase.cpp
 * @brief �������͓���⏕����e���[�h�����̃x�[�X
 * @author SensyuGames
 * @date 2021/12/28
 */

#include "CSDebugAutoPilotModeBase.h"
#include "AutoPilot/CSDebugAutoPilotComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Public/DrawDebugHelpers.h"
#include "Engine/Public/CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerInput.h"

/**
 * @brief	�e�Ƃ���Component���Z�b�g
 */
void	UCSDebugAutoPilotModeBase::SetParent(UCSDebugAutoPilotComponent* InParentComponent)
{
	mAutoPilotComponent = InParentComponent;
	mPlayerController = InParentComponent->GetPlayerController();

	InitializeKeyMap();
	InitializePadDeadZoneMap();
	OnSetParent();
}

/**
 * @brief	�p�b�h���͒l�擾
 */
float	UCSDebugAutoPilotModeBase::GetDebugDrawPadInfoAxisValue(ECSDebugAutoPilotKey InKey) const
{
	for (const FCSDebugAutoPilotDebugDrawPadInfo& Info : mDebugDrawPadInfoList)
	{
		if (Info.mKey == InKey)
		{
			return Info.mAxisValue;
		}
	}
	return 0.f;
}

/**
 * @brief	Key
 */
FKey	UCSDebugAutoPilotModeBase::GetKey(ECSDebugAutoPilotKey InKey) const
{
	if (InKey == ECSDebugAutoPilotKey::Invalid
		|| InKey == ECSDebugAutoPilotKey::Num)
	{
		return FKey();
	}
	const FKey* Key = mKeyMap.Find(InKey);
	if (Key == nullptr)
	{
		return FKey();
	}

	return *Key;
}

/**
 * @brief	�p�b�h���͏�ԃf�o�b�O�\��
 */
void	UCSDebugAutoPilotModeBase::DebugDrawPad(UCanvas* InCanvas)
{
	const float VirtualGridLen = 15.f;//�O���b�h��ɐ}�`�����C���[�W��
	const FVector2D ButtonPosGridList[(uint8)ECSDebugAutoPilotKey::Num] =
	{
		FVector2D::ZeroVector,// Invalid,	//0
		FVector2D(4.0f, 7.0f),// LeftStickX,	//1
		FVector2D(4.0f, 7.0f),// LeftStickY,	//2
		FVector2D(8.0f, 7.0f),// RightStickX,//3
		FVector2D(8.0f, 7.0f),// RightStickY,//4
		FVector2D(2.0f, 4.0f),// Up,			//5
		FVector2D(2.0f, 6.0f),// Down,		//6
		FVector2D(1.0f, 5.0f),// Left,		//7
		FVector2D(3.0f, 5.0f),// Right,		//8
		FVector2D(2.0f, 2.75f),// L1,			//9
		FVector2D(2.0f, 1.5f),// L2,			//10
		FVector2D(4.0f, 7.0f),// L3,			//11
		FVector2D(10.0f,2.75f),// R1,			//12
		FVector2D(10.0f,1.5f),// R2,			//13
		FVector2D(8.0f, 7.0f),// R3,			//14
		FVector2D(10.0f,4.0f),// Sankaku,	//15
		FVector2D(9.0f, 5.0f),// Shikaku,	//16
		FVector2D(10.0f,6.0f),// Batsu,		//17
		FVector2D(11.0f,5.0f),// Maru,		//18
		FVector2D(8.5f, 3.75f),// Option,		//19
		//(5.f,3.5f)�^�b�`�p�b�h��
		//(7.f,3.5f)�^�b�`�p�b�h�E
	};
	const FVector2D ButtonLenGridList[(uint8)ECSDebugAutoPilotKey::Num] =
	{
		FVector2D::UnitVector,// Invalid,	//0
		FVector2D(2.0f, 2.0f),// LeftStickX,	//1
		FVector2D(2.0f, 2.0f),// LeftStickY,	//2
		FVector2D(2.0f, 2.0f),// RightStickX,//3
		FVector2D(2.0f, 2.0f),// RightStickY,//4
		FVector2D(0.5f, 1.0f),// Up,			//5
		FVector2D(0.5f, 1.0f),// Down,		//6
		FVector2D(1.0f, 0.5f),// Left,		//7
		FVector2D(1.0f, 0.5f),// Right,		//8
		FVector2D(1.5f, 0.5f),// L1,			//9
		FVector2D(1.5f, 1.0f),// L2,			//10
		FVector2D(1.0f, 1.0f),// L3,			//11
		FVector2D(1.5f, 0.5f),// R1,			//12
		FVector2D(1.5f, 1.0f),// R2,			//13
		FVector2D(1.0f, 1.0f),// R3,			//14
		FVector2D(0.5f, 0.5f),// Sankaku,	//15
		FVector2D(0.5f, 0.5f),// Shikaku,	//16
		FVector2D(0.5f, 0.5f),// Batsu,		//17
		FVector2D(0.5f, 0.5f),// Maru,		//18
		FVector2D(0.5f, 0.5f),// Option,		//19
		//(1.0f,1.0f)�^�b�`�p�b�h��
		//(1.0f,1.0f)�^�b�`�p�b�h�E
	};

	const FVector2D Extent(VirtualGridLen*12.f, VirtualGridLen*9.f);
	FVector2D ScreenPos(50.f, 50.f);
	if (GEngine
		&& GEngine->GameViewport)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		ScreenPos.X = ViewportSize.X*0.5f - Extent.X*0.5f;
	}
	DebugDrawPadSheet(InCanvas, ScreenPos, Extent);


	for (uint8 i = 1; i < (uint8)ECSDebugAutoPilotKey::Num; ++i)
	{
		const ECSDebugAutoPilotKey KeyId = static_cast<ECSDebugAutoPilotKey>(i);

		const FVector2D CenterPos = ScreenPos + ButtonPosGridList[i] * VirtualGridLen;
		const FVector2D ExtenetV = ButtonLenGridList[i] * VirtualGridLen * 0.5f;

		//stick�͓���Ώ�
		if (KeyId == ECSDebugAutoPilotKey::LeftStickX)
		{
			FVector2D AxisV(GetDebugDrawPadInfoAxisValue(ECSDebugAutoPilotKey::LeftStickX), GetDebugDrawPadInfoAxisValue(ECSDebugAutoPilotKey::LeftStickY));
			DebugDrawStick(InCanvas, AxisV, CenterPos, ExtenetV.X);
		}
		else if (KeyId == ECSDebugAutoPilotKey::RightStickX)
		{
			FVector2D AxisV(GetDebugDrawPadInfoAxisValue(ECSDebugAutoPilotKey::RightStickX), GetDebugDrawPadInfoAxisValue(ECSDebugAutoPilotKey::RightStickY));
			AxisV *= FVector2D(1.f, -1.f);//���̂��t�ɂȂ�̂�(���������ď�,�t�̃I�v�V�����̉e���󂯂Ă�H)
			DebugDrawStick(InCanvas, AxisV, CenterPos, ExtenetV.X);
		}
		else if (KeyId == ECSDebugAutoPilotKey::LeftStickY
			|| KeyId == ECSDebugAutoPilotKey::RightStickY)
		{	//x�ɔC����̂�
			continue;;
		}
		else if (KeyId == ECSDebugAutoPilotKey::L3
			|| KeyId == ECSDebugAutoPilotKey::R3)
		{
			if (GetDebugDrawPadInfoAxisValue(KeyId) > 0.f)
			{
				FLinearColor FrameColor = FLinearColor(1.f, 0.5f, 0.f);
				DrawDebugCanvas2DCircle(InCanvas, CenterPos, ExtenetV.X, 10, FrameColor, 2.f);
			}
		}
		else
		{
			DebugDrawButton(InCanvas, KeyId, CenterPos, ExtenetV);
		}
	}

	mDebugDrawPadInfoList.Reset();
}

/**
 * @brief	KeyMap�p��
 */
void UCSDebugAutoPilotModeBase::InitializeKeyMap()
{
	if (mKeyMap.Num() != 0)
	{
		return;
	}

	mKeyMap.Reserve((int32)ECSDebugAutoPilotKey::Num);

	mKeyMap.Add(ECSDebugAutoPilotKey::LeftStickX, EKeys::Gamepad_LeftX);
	mKeyMap.Add(ECSDebugAutoPilotKey::LeftStickY, EKeys::Gamepad_LeftY);
	mKeyMap.Add(ECSDebugAutoPilotKey::RightStickX, EKeys::Gamepad_RightX);
	mKeyMap.Add(ECSDebugAutoPilotKey::RightStickY, EKeys::Gamepad_RightY);
	mKeyMap.Add(ECSDebugAutoPilotKey::Up, EKeys::Gamepad_DPad_Up);
	mKeyMap.Add(ECSDebugAutoPilotKey::Down, EKeys::Gamepad_DPad_Down);
	mKeyMap.Add(ECSDebugAutoPilotKey::Left, EKeys::Gamepad_DPad_Left);
	mKeyMap.Add(ECSDebugAutoPilotKey::Right, EKeys::Gamepad_DPad_Right);
	mKeyMap.Add(ECSDebugAutoPilotKey::L1, EKeys::Gamepad_LeftShoulder);
	mKeyMap.Add(ECSDebugAutoPilotKey::L2, EKeys::Gamepad_LeftTrigger);
	mKeyMap.Add(ECSDebugAutoPilotKey::L3, EKeys::Gamepad_LeftThumbstick);
	mKeyMap.Add(ECSDebugAutoPilotKey::R1, EKeys::Gamepad_RightShoulder);
	mKeyMap.Add(ECSDebugAutoPilotKey::R2, EKeys::Gamepad_RightTrigger);
	mKeyMap.Add(ECSDebugAutoPilotKey::R3, EKeys::Gamepad_RightThumbstick);
	mKeyMap.Add(ECSDebugAutoPilotKey::Sankaku, EKeys::Gamepad_FaceButton_Top);
	mKeyMap.Add(ECSDebugAutoPilotKey::Shikaku, EKeys::Gamepad_FaceButton_Left);
	mKeyMap.Add(ECSDebugAutoPilotKey::Batsu, EKeys::Gamepad_FaceButton_Bottom);
	mKeyMap.Add(ECSDebugAutoPilotKey::Maru, EKeys::Gamepad_FaceButton_Right);
	//mKeyMap.Add(&EKeys::Gamepad_Special_Right, ECSDebugAutoPilotKey::Option, );
}

/**
 * @brief	DeadZoneMap�p��
 */
void UCSDebugAutoPilotModeBase::InitializePadDeadZoneMap()
{
	if (mPadDeadZoneMap.Num() > 0)
	{
		return;
	}

	const APlayerController* PlayerControler = GetPlayerController();
	if (const UPlayerInput* PlayerInput = PlayerControler->PlayerInput)
	{
		for (const FInputAxisConfigEntry& AxisConfigEntry : PlayerInput->AxisConfig)
		{
			const FKey AxisKey = AxisConfigEntry.AxisKeyName;
			if (AxisKey.IsAxis1D())
			//if (AxisKey.IsFloatAxis())
			{
				mPadDeadZoneMap.Add(AxisKey, AxisConfigEntry.AxisProperties.DeadZone);
			}
		}
	}
}

/**
 * @brief	�p�b�h���͏�ԃf�o�b�O�\��(���~��)
 */
void	UCSDebugAutoPilotModeBase::DebugDrawPadSheet(UCanvas* InCanvas, const FVector2D& InBasePos, const FVector2D& InExtent)
{
	const FLinearColor FrameColor = { 0.5f, 1.f, 0.5f, 1.f };
	const FLinearColor BackColor = { 0.f, 0.f, 0.f, 0.4f };
	const uint32 WindowPointListSize = 4;
	const FVector2D WindowPointList[WindowPointListSize] =
	{
		FVector2D(InBasePos.X, InBasePos.Y),//����
		FVector2D(InBasePos.X, InBasePos.Y + InExtent.Y),//����
		FVector2D(InBasePos.X + InExtent.X, InBasePos.Y + InExtent.Y),//�E��
		FVector2D(InBasePos.X + InExtent.X, InBasePos.Y)//�E��
	};
	for (uint32 i = 0; i < WindowPointListSize; ++i)
	{//�g
		DrawDebugCanvas2DLine(InCanvas, WindowPointList[i], WindowPointList[(i + 1) % WindowPointListSize], FrameColor);
	}
	{//���~��
		FCanvasTileItem TileItem(InBasePos, GWhiteTexture, FVector2D(InExtent.X, InExtent.Y), BackColor);
		TileItem.BlendMode = SE_BLEND_Translucent;
		InCanvas->DrawItem(TileItem);
	}
}
/**
 * @brief	2D���\��
 */
void	UCSDebugAutoPilotModeBase::DebugDrawArrow2D(UCanvas* InCanvas, const FVector2D& InStartPos, const FVector2D& InGoalPos, const FLinearColor& InColor, float InArrowLen)
{
	DrawDebugCanvas2DLine(InCanvas, InStartPos, InGoalPos, InColor);

	FVector2D StartNV = InStartPos - InGoalPos;
	StartNV.Normalize();
	{
		FVector ArrowV(StartNV*InArrowLen, 0.f);
		ArrowV = ArrowV.RotateAngleAxis(45.f, FVector(0.f, 0.f, 1.f));
		FVector2D ArrowGoalPos(InGoalPos + FVector2D(ArrowV));
		DrawDebugCanvas2DLine(InCanvas, InGoalPos, ArrowGoalPos, InColor);
	}
	{
		FVector ArrowV(StartNV*InArrowLen, 0.f);
		ArrowV = ArrowV.RotateAngleAxis(-45.f, FVector(0.f, 0.f, 1.f));
		FVector2D ArrowGoalPos(InGoalPos + FVector2D(ArrowV));
		DrawDebugCanvas2DLine(InCanvas, InGoalPos, ArrowGoalPos, InColor);
	}
}
/**
 * @brief	�{�^���\��
 */
void	UCSDebugAutoPilotModeBase::DebugDrawButton(UCanvas* InCanvas, ECSDebugAutoPilotKey InKey, const FVector2D& InPos, const FVector2D& InExtent)
{
	const FLinearColor FrameColor = FLinearColor::White;
	FLinearColor BackColor = { 0.f, 0.f, 0.f, 0.4f };
	if (GetDebugDrawPadInfoAxisValue(InKey) > 0.f)
	{
		BackColor = FLinearColor(1.f, 0.5f, 0.f);
	}

	const uint32 WindowPointListSize = 4;
	const FVector2D WindowPointList[WindowPointListSize] =
	{
		FVector2D(InPos.X - InExtent.X, InPos.Y - InExtent.Y),//����
		FVector2D(InPos.X - InExtent.X, InPos.Y + InExtent.Y),//����
		FVector2D(InPos.X + InExtent.X, InPos.Y + InExtent.Y),//�E��
		FVector2D(InPos.X + InExtent.X, InPos.Y - InExtent.Y)//�E��
	};
	for (uint32 i = 0; i < WindowPointListSize; ++i)
	{//�g
		DrawDebugCanvas2DLine(InCanvas, WindowPointList[i], WindowPointList[(i + 1) % WindowPointListSize], FrameColor);
	}
	{//���~��
		FCanvasTileItem TileItem(FVector2D(InPos.X - InExtent.X, InPos.Y - InExtent.Y), GWhiteTexture, FVector2D(InExtent.X, InExtent.Y)*2.f, BackColor);
		TileItem.BlendMode = SE_BLEND_Translucent;
		InCanvas->DrawItem(TileItem);
	}
}
/**
 * @brief	�X�e�B�b�N�\��
 */
void	UCSDebugAutoPilotModeBase::DebugDrawStick(UCanvas* InCanvas, const FVector2D& InAxisV, const FVector2D& InPos, const float InRadius)
{
	const FLinearColor FrameColor = FLinearColor::White;
	DrawDebugCanvas2DCircle(InCanvas, InPos, InRadius, 10, FrameColor);

	FLinearColor ArrowColor = FLinearColor::White;
	if (InAxisV.SizeSquared() > 0.f)
	{
		ArrowColor = FLinearColor(1.f, 0.5f, 0.f);
	}

	FVector2D ArrowPos = InPos + InAxisV * InRadius * FVector2D(1.f, -1.f);
	DebugDrawArrow2D(InCanvas, InPos, ArrowPos, ArrowColor, 2.5f);
}