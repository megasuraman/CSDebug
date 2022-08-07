// Copyright 2021 SensyuGames.
/**
 * @file CSDebugAutoPilotModeBase.h
 * @brief 自動入力等を補助する各モード処理のベース
 * @author SensyuGames
 * @date 2021/12/28
 */

#pragma once

#include "CoreMinimal.h"
#include "Serialization/JsonSerializerMacros.h"
#include "InputCore/Classes/InputCoreTypes.h"
#include "UObject/NoExportTypes.h"
#include "CSDebugAutoPilotModeBase.generated.h"

class UCanvas;
class UCSDebugAutoPilotComponent;

enum class ECSDebugAutoPilotKey : uint8 {
	Invalid,
	LeftStickX,
	LeftStickY,
	RightStickX,
	RightStickY,
	Up,
	Down,
	Left,
	Right,
	L1,
	L2,
	L3,
	R1,
	R2,
	R3,
	Sankaku,
	Shikaku,
	Batsu,
	Maru,
	Option,
	Num,
};

struct FCSDebugAutoPilotDebugDrawPadInfo
{
	ECSDebugAutoPilotKey	mKey;
	float	mAxisValue = 0.f;
	FCSDebugAutoPilotDebugDrawPadInfo(ECSDebugAutoPilotKey InKey, float InValue)
		:mKey(InKey)
		, mAxisValue(InValue)
	{}
};

UCLASS()
class CSDEBUG_API UCSDebugAutoPilotModeBase : public UObject
{
	GENERATED_BODY()

public:
	void	SetParent(UCSDebugAutoPilotComponent* InParentComponent);

	virtual void	PreProcessInput(float DeltaTime) {}
	virtual void	PostProcessInput(float DeltaTime) {}

	virtual void	DebugDraw(class UCanvas* InCanvas) {}

protected:
	virtual void	OnSetParent() {}
	UCSDebugAutoPilotComponent* GetParent() { return mAutoPilotComponent; }
	class APlayerController* GetPlayerController() const { return mPlayerController; }
	float	GetDebugDrawPadInfoAxisValue(ECSDebugAutoPilotKey InKey) const;
	const TMap<FKey, float>& GetPadDeadZoneMap() const { return mPadDeadZoneMap; }
	FKey GetKey(ECSDebugAutoPilotKey InKey) const;

	void	AddDebugDrawPadInfo(const FCSDebugAutoPilotDebugDrawPadInfo& InInfo)
	{
		mDebugDrawPadInfoList.Add(InInfo);
	}

	void	DebugDrawPad(UCanvas* InCanvas);

private:
	void	InitializeKeyMap();
	void	InitializePadDeadZoneMap();

	void	DebugDrawPadSheet(UCanvas* InCanvas, const FVector2D& InBasePos, const FVector2D& InExtent);
	void	DebugDrawArrow2D(UCanvas* InCanvas, const FVector2D& InStartPos, const FVector2D& InGoalPos, const FLinearColor& InColor, float InArrowLen);
	void	DebugDrawButton(UCanvas* InCanvas, ECSDebugAutoPilotKey InKey, const FVector2D& InPos, const FVector2D& InExtent);
	void	DebugDrawStick(UCanvas* InCanvas, const FVector2D& InAxisV, const FVector2D& InPos, const float InRadius);


private:
	TMap<ECSDebugAutoPilotKey, FKey>	mKeyMap;
	TMap<FKey, float>	mPadDeadZoneMap;
	TArray<FCSDebugAutoPilotDebugDrawPadInfo>	mDebugDrawPadInfoList;//パッド入力のデバッグ表示用に
	UCSDebugAutoPilotComponent* mAutoPilotComponent = nullptr;
	APlayerController*	mPlayerController = nullptr;
};