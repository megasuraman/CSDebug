// Copyright 2022 SensyuGames.

#pragma once

#include "CoreMinimal.h"
#include "DebugMenu/CSDebug_DebugMenuTableRow.h"
#include "DebugMenu/CSDebug_DebugMenuNodeButton.h"

class CSDEBUG_API CSDebug_DebugMenuNodeFolder : public CSDebug_DebugMenuNodeButton
{
public:
	virtual void OnEndAction(const FCSDebug_DebugMenuNodeActionParameter& InParameter) override;
	virtual void OnJustPressedRightKey() override;

protected:
	virtual void DrawValue(UCanvas* InCanvas, const FVector2D& InPos, const FLinearColor InColor) const;

private:
};
