// Copyright 2022 SensyuGames.

#pragma once

#include "CoreMinimal.h"
#include "DebugMenu/CSDebug_DebugMenuTableRow.h"
#include "DebugMenu/CSDebug_DebugMenuNodeBase.h"

class CSDEBUG_API CSDebug_DebugMenuNodeInt : public CSDebug_DebugMenuNodeBase
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
	static const int32 mEditDigitNum = 8;//åÖêî
	static const int32 mEditDigitIntListNum = mEditDigitNum + 1;//ïÑçÜï™í«â¡
	TArray<int32> mEditDigitIntList;
	int32 mEditDigitIntIndex = 0;//mEditDigitIntListÇÃï“èWà íu
};
