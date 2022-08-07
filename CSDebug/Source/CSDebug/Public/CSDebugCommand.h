// Copyright 2020 SensyuGames.
/**
 * @file CSDebugCommand.h
 * @brief DebugCommandの管理クラス
 * @author SensyuGames
 * @date 2020/5/25
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebugCommand.generated.h"


class APlayerController;

/**
 * 
 */
UCLASS()
class CSDEBUG_API UCSDebugCommand : public UObject
{
	GENERATED_BODY()

	UCSDebugCommand();

#if USE_CSDEBUG
public:
	void	Init();
	bool	DebugTick(float InDeltaSecond);
	void	DebugDraw(class UCanvas* InCanvas);

protected:
	APlayerController* FindPlayerController() const;

	void	CheckDebugStep(APlayerController* InPlayerController, float InDeltaSecond);
	void	CheckDebugCameraMode(APlayerController* InPlayerController);

	void	SwicthDebugMenuActive(APlayerController* InPlayerController);
	void	SetDebugStop(bool bInStop, APlayerController* InPlayerController);
	void	SetStopMotionBlur(bool bInStop, APlayerController* InPlayerController);

private:
	float	mDebugStepRepeatBeginSec = 0.3f;//DebugStepRepeat発動までの押しっぱなし時間
	float	mDebugStepRepeatBeginTimer = 0.f;//DebugStepRepeat発動までの押しっぱなし計測時間
	float	mDebugStepInterval = 0.f;//DebugStepでPause解除後に再度解除するまでの時間
	float	mDebugStepRepeatStopSec = 0.05f;//DebugStepRepeat時の停止時間
	float	mDebugStepRepeatStopTimer = 0.f;//DebugStepRepeat時の停止計測時間
	bool	mbRequestDebugStopOnDebugMenu = false;//DebugMenuと同時にDebugStop
	uint8	mbDebugStopMode : 1;//DebugStopModeかどうか
	uint8	mbDebugCameraMode : 1;//DebugCameraModeかどうか
	uint8	mbDebugStop : 1;//実際にPause中かどうか
	uint8	mbDebugStep : 1;//一瞬DebugStop解除して再度DebugStopしたいかどうか
	uint8	mbDebugStepRepeat : 1;//DebugStepを繰り返したいかどうか
	uint8	mbStopMotionBlur : 1;//DebugStopとDebugCameraを併用時にモーションブラー(カメラブラー)が邪魔なのでon/offように
	uint8	mbRequestReleaseDebugStopAfterMenu : 1;//DebugMenu終了時にDebugStop解除
#endif //USE_CSDEBUG
};
