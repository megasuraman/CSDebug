// Copyright 2020 SensyuGames.
/**
 * @file CSDebugCommand.h
 * @brief DebugCommand�̊Ǘ��N���X
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
	float	mDebugStepRepeatBeginSec = 0.3f;//DebugStepRepeat�����܂ł̉������ςȂ�����
	float	mDebugStepRepeatBeginTimer = 0.f;//DebugStepRepeat�����܂ł̉������ςȂ��v������
	float	mDebugStepInterval = 0.f;//DebugStep��Pause������ɍēx��������܂ł̎���
	float	mDebugStepRepeatStopSec = 0.05f;//DebugStepRepeat���̒�~����
	float	mDebugStepRepeatStopTimer = 0.f;//DebugStepRepeat���̒�~�v������
	bool	mbRequestDebugStopOnDebugMenu = false;//DebugMenu�Ɠ�����DebugStop
	uint8	mbDebugStopMode : 1;//DebugStopMode���ǂ���
	uint8	mbDebugCameraMode : 1;//DebugCameraMode���ǂ���
	uint8	mbDebugStop : 1;//���ۂ�Pause�����ǂ���
	uint8	mbDebugStep : 1;//��uDebugStop�������čēxDebugStop���������ǂ���
	uint8	mbDebugStepRepeat : 1;//DebugStep���J��Ԃ��������ǂ���
	uint8	mbStopMotionBlur : 1;//DebugStop��DebugCamera�𕹗p���Ƀ��[�V�����u���[(�J�����u���[)���ז��Ȃ̂�on/off�悤��
	uint8	mbRequestReleaseDebugStopAfterMenu : 1;//DebugMenu�I������DebugStop����
#endif //USE_CSDEBUG
};
