// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMenuListObjectBase.cpp
 * @brief DebugMenu用WidgetのListViewに使うObjectクラス
 * @author SensyuGames
 * @date 2020/08/21
 */

#include "DebugMenu/CSDebugMenuListObjectBase.h"
#include "DebugMenu/CSDebugMenuWidgetBase.h"

/**
 * @brief	初期化処理
 */
void	UCSDebugMenuListObjectBase::Init(const FCSDebugMenuNodeValue& InNodeValue)
{
	mNodeValue = InNodeValue;
	SetNodeName(mNodeValue.mDispString);
	SetValueName(mNodeValue.GetValueString());
}

/**
 * @brief	決定時処理
 */
void	UCSDebugMenuListObjectBase::OnDecide()
{
	const ECSDebugMenuNodeValueKind ValueKind = mNodeValue.GetKind();
	switch (ValueKind)
	{
	case ECSDebugMenuNodeValueKind::Bool:
		mNodeValue.Set(!mNodeValue.GetBool());
		break;
	case ECSDebugMenuNodeValueKind::Menu:
	{
		if (UCSDebugMenuWidgetBase* DebugMenu = mDebugMenuWidget.Get())
		{
			DebugMenu->RequestChangeNextMenu();
		}
		break;
	}
	case ECSDebugMenuNodeValueKind::Int:
	{
		if (UCSDebugMenuWidgetBase* DebugMenu = mDebugMenuWidget.Get())
		{
			DebugMenu->RequestBeginNodeInt();
		}
		break;
	}
	case ECSDebugMenuNodeValueKind::Float:
	{
		if (UCSDebugMenuWidgetBase* DebugMenu = mDebugMenuWidget.Get())
		{
			DebugMenu->RequestBeginNodeFloat();
		}
		break;
	}
	case ECSDebugMenuNodeValueKind::Selector:
	{
		if (UCSDebugMenuWidgetBase* DebugMenu = mDebugMenuWidget.Get())
		{
			DebugMenu->RequestBeginNodeSelector();
		}
		break;
	}
	case ECSDebugMenuNodeValueKind::Button:
	{
		mNodeValue.CallDelegate();
		break;
	}
	default:
		break;
	}

	SetValueName(mNodeValue.GetValueString());
	OnDecideBP();
}

/**
 * @brief	リスト型データのリスト取得
 */
TArray<FString>	UCSDebugMenuListObjectBase::GetSelectorItemList() const
{
	FCSDebugMenuNodeValueKindSelector SelectorList;
	mNodeValue.GetSelectorList(SelectorList);
	return SelectorList.mList;
}
/**
 * @brief	リスト型データのIndex取得
 */
int32	UCSDebugMenuListObjectBase::GetSelectorIndex() const
{
	FCSDebugMenuNodeValueKindSelector SelectorList;
	mNodeValue.GetSelectorList(SelectorList);
	return SelectorList.mIndex;
}

/**
 * @brief	選択時処理
 */
void	UCSDebugMenuListObjectBase::OnSelect()
{
	OnSelectBP();
}

/**
 * @brief	選択解除時処理
 */
void	UCSDebugMenuListObjectBase::OnNoSelect()
{
	OnNoSelectBP();
}

/**
 * @brief	キャンセル時処理
 */
void	UCSDebugMenuListObjectBase::OnCancel()
{
}

/**
 * @brief	値セット
 */
void	UCSDebugMenuListObjectBase::SetValueBool(const bool bInBool)
{
	mNodeValue.Set(bInBool);
	SetValueName(mNodeValue.GetValueString());
}
void	UCSDebugMenuListObjectBase::SetValueInt(int32 InInt)
{
	mNodeValue.Set(InInt);
	SetValueName(mNodeValue.GetValueString());
}
void	UCSDebugMenuListObjectBase::SetValueFloat(float InFloat)
{
	mNodeValue.Set(InFloat);
	SetValueName(mNodeValue.GetValueString());
}
void	UCSDebugMenuListObjectBase::SetValueSelectorListIndex(int32 InListIndex)
{
	mNodeValue.SetSelectorListIndex(InListIndex);
	SetValueName(mNodeValue.GetValueString());
}
/**
 * @brief	値文字列設定
 */
void	UCSDebugMenuListObjectBase::SetValueName(const FString& InString)
{
	mValueName = InString;
	OnSetValueNameBP();
}
/**
 * @brief	値の種類
 */
ECSDebugMenuNodeValueKind	UCSDebugMenuListObjectBase::GetNodeVaueKind() const
{
	return mNodeValue.GetKind();
}

#if WITH_EDITOR
/**
 * @brief	EUW用に値セット
 */
void	UCSDebugMenuListObjectBase::EUWSetupValue(const FCSDebugMenuNodeEUW& InData)
{
	InData.GetNodeValue(mNodeValue);
	SetNodeName(mNodeValue.mDispString);
	SetValueName(mNodeValue.GetValueString());
}
/**
 * @brief	mNodeValueのjson文字列取得
 */
FString	UCSDebugMenuListObjectBase::EUWGetNodeValueJsonString() const
{
	return mNodeValue.ToJson();
}
/**
 * @brief	値の種類変更
 */
void	UCSDebugMenuListObjectBase::EUWChangeValueKind(ECSDebugMenuNodeValueKind InKind)
{
	if (mNodeValue.mKind == (int32)InKind)
	{
		return;
	}
	mNodeValue.mKind = (int32)InKind;
	mNodeValue.mValueString = FString();
}
/**
 * @brief	セーブ用にSelectorのValueStringはそのまま取得
 */
FString	UCSDebugMenuListObjectBase::EUWGetValueString() const
{
	if (GetNodeVaueKind() == ECSDebugMenuNodeValueKind::Selector)
	{
		return mNodeValue.mValueString;
	}
	return GetValueName();
}
/**
 * @brief	Selectorの設定上書き
 */
void	UCSDebugMenuListObjectBase::EUWOverrideSelector(const TArray<FString>& InSelector, const int32 InIndex)
{
	if (mNodeValue.GetKind() != ECSDebugMenuNodeValueKind::Selector)
	{
		return;
	}
	mNodeValue.Set(InIndex, InSelector);
}
#endif//WITH_EDITOR