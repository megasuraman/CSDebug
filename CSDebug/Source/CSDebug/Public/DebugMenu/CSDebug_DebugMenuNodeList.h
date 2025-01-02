// Copyright 2022 SensyuGames.

#pragma once

#include "CoreMinimal.h"
#include "DebugMenu/CSDebug_DebugMenuTableRow.h"
#include "DebugMenu/CSDebug_DebugMenuNodeBase.h"

class CSDEBUG_API CSDebug_DebugMenuNodeList : public CSDebug_DebugMenuNodeBase
{
public:
	virtual void OnBeginAction() override;
	virtual void OnEndAction(const FCSDebug_DebugMenuNodeActionParameter& InParameter) override;
	virtual void OnJustPressedUpKey() override;
	virtual void OnJustPressedDownKey() override;
	virtual void OnJustPressedLeftKey() override;
	virtual void OnJustPressedRightKey() override;

protected:
	virtual void SetInitValue() override;
	virtual void DrawEditValue(UCanvas* InCanvas, const FVector2D& InPos, const FVector2D& InValueExtent) const;

private:
	static const int32 mEditDrawListNum = 9;//�\�����郊�X�g���ڐ�
	static const int32 mEditDrawListInitCursolOffset = mEditDrawListNum/2;//�I���J�[�\���̏����I�t�Z�b�g
	int32 mEditSelectIndex = 0;
	int32 mEditDrawIndexMin = 0;
	int32 mEditDrawIndexMax = 0;
};
