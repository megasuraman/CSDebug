// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMenuListObjectBase.cpp
 * @brief DebugMenu�pWidget��ListView�Ɏg��Object�N���X
 * @author SensyuGames
 * @date 2020/08/21
 */

#include "DebugMenu/CSDebugMenuListObjectBase.h"
#include "DebugMenu/CSDebugMenuWidgetBase.h"

/**
 * @brief	����������
 */
void	UCSDebugMenuListObjectBase::Init(const FCSDebugMenuNodeValue& InNodeValue)
{
	mNodeValue = InNodeValue;
	SetNodeName(mNodeValue.mDispString);
	SetValueName(mNodeValue.GetValueString());
}

/**
 * @brief	���莞����
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
 * @brief	���X�g�^�f�[�^�̃��X�g�擾
 */
TArray<FString>	UCSDebugMenuListObjectBase::GetSelectorItemList() const
{
	FCSDebugMenuNodeValueKindSelector SelectorList;
	mNodeValue.GetSelectorList(SelectorList);
	return SelectorList.mList;
}
/**
 * @brief	���X�g�^�f�[�^��Index�擾
 */
int32	UCSDebugMenuListObjectBase::GetSelectorIndex() const
{
	FCSDebugMenuNodeValueKindSelector SelectorList;
	mNodeValue.GetSelectorList(SelectorList);
	return SelectorList.mIndex;
}

/**
 * @brief	�I��������
 */
void	UCSDebugMenuListObjectBase::OnSelect()
{
	OnSelectBP();
}

/**
 * @brief	�I������������
 */
void	UCSDebugMenuListObjectBase::OnNoSelect()
{
	OnNoSelectBP();
}

/**
 * @brief	�L�����Z��������
 */
void	UCSDebugMenuListObjectBase::OnCancel()
{
}

/**
 * @brief	�l�Z�b�g
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
 * @brief	�l������ݒ�
 */
void	UCSDebugMenuListObjectBase::SetValueName(const FString& InString)
{
	mValueName = InString;
	OnSetValueNameBP();
}
/**
 * @brief	�l�̎��
 */
ECSDebugMenuNodeValueKind	UCSDebugMenuListObjectBase::GetNodeVaueKind() const
{
	return mNodeValue.GetKind();
}

#if WITH_EDITOR
/**
 * @brief	EUW�p�ɒl�Z�b�g
 */
void	UCSDebugMenuListObjectBase::EUWSetupValue(const FCSDebugMenuNodeEUW& InData)
{
	InData.GetNodeValue(mNodeValue);
	SetNodeName(mNodeValue.mDispString);
	SetValueName(mNodeValue.GetValueString());
}
/**
 * @brief	mNodeValue��json������擾
 */
FString	UCSDebugMenuListObjectBase::EUWGetNodeValueJsonString() const
{
	return mNodeValue.ToJson();
}
/**
 * @brief	�l�̎�ޕύX
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
 * @brief	�Z�[�u�p��Selector��ValueString�͂��̂܂܎擾
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
 * @brief	Selector�̐ݒ�㏑��
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