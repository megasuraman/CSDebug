// Copyright 2022 SensyuGames.

#include "DebugMenu/CSDebug_DebugMenuNodeBool.h"


void CSDebug_DebugMenuNodeBool::OnEndAction(const FCSDebug_DebugMenuNodeActionParameter& InParameter)
{
	CSDebug_DebugMenuNodeBase::OnEndAction(InParameter);
	SetValueBool(!GetBool());
}

void CSDebug_DebugMenuNodeBool::SetInitValue()
{
	if (!GetNodeData().mInitValue.IsEmpty())
	{
		SetValueString(GetNodeData().mInitValue);
		SetValueBool(GetBool());
	}
}
