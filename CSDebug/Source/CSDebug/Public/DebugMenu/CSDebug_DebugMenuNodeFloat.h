// Copyright 2022 SensyuGames.

#pragma once

#include "CoreMinimal.h"
#include "DebugMenu/CSDebug_DebugMenuTableRow.h"
#include "DebugMenu/CSDebug_DebugMenuNodeBase.h"

class CSDEBUG_API CSDebug_DebugMenuNodeFloat : public CSDebug_DebugMenuNodeBase
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
	static const int32 mEditIntegralDigitNum = 7;//ฎ
	static const int32 mEditDecimalNum = 3;//ฌ
	static const int32 mEditDigitNumberNum = mEditIntegralDigitNum + mEditDecimalNum;//
	static const int32 mEditDigitNum = mEditDigitNumberNum + 1;//ชวม
	TArray<int32> mEditDigitIntList;
	int32 mEditDigitIndex = 0;//mEditDigitIntListฬาWสu
};
