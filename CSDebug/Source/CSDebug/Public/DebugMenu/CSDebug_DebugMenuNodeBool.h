// Copyright 2022 SensyuGames.

#pragma once

#include "CoreMinimal.h"
#include "DebugMenu/CSDebug_DebugMenuTableRow.h"
#include "DebugMenu/CSDebug_DebugMenuNodeBase.h"

class CSDEBUG_API CSDebug_DebugMenuNodeBool : public CSDebug_DebugMenuNodeBase
{
public:
	virtual void OnEndAction(const FCSDebug_DebugMenuNodeActionParameter& InParameter) override;

protected:
	virtual void SetInitValue() override;

private:
};
