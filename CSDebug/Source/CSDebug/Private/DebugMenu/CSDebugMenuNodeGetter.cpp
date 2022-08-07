// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMenuNodeGetter.cpp
 * @brief CSDebugMenuÇÃäOïîéQè∆ópNodeèÓïÒ
 * @author SensyuGames
 * @date 2020/08/01
 */

#include "DebugMenu/CSDebugMenuNodeGetter.h"


FCSDebugMenuNodeGetter::FCSDebugMenuNodeGetter(const FCSDebugMenuNodeValue& InValue)
{
	mValueString = InValue.GetValueString();
}

bool	FCSDebugMenuNodeGetter::GetBool() const
{
	return mValueString.ToBool();
}
int32	FCSDebugMenuNodeGetter::GetInt() const
{
	return FCString::Atoi(*mValueString);
}
float	FCSDebugMenuNodeGetter::GetFloat() const
{
	return FCString::Atof(*mValueString);
}
int32	FCSDebugMenuNodeGetter::GetSelectIndex() const
{
	FCSDebugMenuNodeValueKindSelector ValueSelector;
	ValueSelector.FromJson(mValueString);
	return ValueSelector.mIndex;
}
FString	FCSDebugMenuNodeGetter::GetSelectString() const
{
	FCSDebugMenuNodeValueKindSelector ValueSelector;
	ValueSelector.FromJson(mValueString);
	if (ValueSelector.mIndex < ValueSelector.mList.Num())
	{
		return ValueSelector.mList[ValueSelector.mIndex];
	}
	return FString();
}


FCSDebugMenuNodeEUW::FCSDebugMenuNodeEUW(const FCSDebugMenuNodeValue& InValue)
{
	mOriginJsonString = InValue.ToJson();
	mDispString = InValue.GetDispString();
	mValueString = InValue.mValueString;//FCSDebugMenuNodeValueKindSelectorÇÃjsonèÓïÒó~ÇµÇ¢ÇÃÇ≈
	mKind = (int32)InValue.GetKind();
	mbFromEUW = true;
}

void	FCSDebugMenuNodeEUW::GetNodeValue(FCSDebugMenuNodeValue& OutNode) const
{
	FCSDebugMenuNodeValue OriginNodeValue;
	if (OriginNodeValue.FromJson(mOriginJsonString))
	{
		OutNode.mList = OriginNodeValue.mList;
	}
	OutNode.mDispString = mDispString;
	OutNode.mValueString = mValueString;
	OutNode.mKind = mKind;
	OutNode.mbFromEUW = mbFromEUW;
}