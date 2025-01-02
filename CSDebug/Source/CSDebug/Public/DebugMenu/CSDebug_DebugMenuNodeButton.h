// Copyright 2022 SensyuGames.

#pragma once

#include "CoreMinimal.h"
#include "DebugMenu/CSDebug_DebugMenuTableRow.h"
#include "DebugMenu/CSDebug_DebugMenuNodeBase.h"

class CSDEBUG_API CSDebug_DebugMenuNodeButton : public CSDebug_DebugMenuNodeBase
{
public:
	virtual void OnEndAction(const FCSDebug_DebugMenuNodeActionParameter& InParameter) override;

protected:
	virtual void DrawValue(UCanvas* InCanvas, const FVector2D& InPos, const FLinearColor InColor) const;

};
