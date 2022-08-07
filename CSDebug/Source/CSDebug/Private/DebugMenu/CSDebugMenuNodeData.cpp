// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMenuNodeData.cpp
 * @brief CSDebugMenuのNode情報
 * @author SensyuGames
 * @date 2020/08/01
 */

#include "DebugMenu/CSDebugMenuNodeData.h"
#include "DebugMenu/CSDebugMenuNodeGetter.h"


/**
 * @brief	ロード後にプログラムから上書きセット
 */
void	FCSDebugMenuNodeValue::SetRuntimeNode(const FCSDebugMenuNodeValue& InNodeValue)
{
	mList = InNodeValue.mList;
	mDispString = InNodeValue.mDispString;
	//mValueString = InNodeValue.mValueString;ロード側を残すために
	mKind = InNodeValue.mKind;
	mDelegate = InNodeValue.mDelegate;
	mbFromEUW = InNodeValue.mbFromEUW;
	mbAddRuntime = true;

	//ロードの値を反映させるために
	const ECSDebugMenuNodeValueKind Kind = GetKind();
	if (Kind == ECSDebugMenuNodeValueKind::Bool
		|| Kind == ECSDebugMenuNodeValueKind::Int
		|| Kind == ECSDebugMenuNodeValueKind::Float
		|| Kind == ECSDebugMenuNodeValueKind::Selector
		)
	{
		CallDelegate();
	}
}

/**
 * @brief	FCSDebugMenuNodeValueの値の文字列取得
 */
FString	FCSDebugMenuNodeValue::GetValueString() const
{
	const ECSDebugMenuNodeValueKind ValueKind = GetKind();
	switch (ValueKind)
	{
	case ECSDebugMenuNodeValueKind::Bool:
	case ECSDebugMenuNodeValueKind::Int:
	case ECSDebugMenuNodeValueKind::Float:
		return mValueString;
	case ECSDebugMenuNodeValueKind::Menu:
		break;
	case ECSDebugMenuNodeValueKind::Selector:
	{
		FCSDebugMenuNodeValueKindSelector ListData;
		if (GetSelectorList(ListData)
			&& ListData.mIndex < ListData.mList.Num())
		{
			return ListData.mList[ListData.mIndex];
		}
		break;
	}
	default:
		break;
	}
	return FString();
}
/**
 * @brief	FCSDebugMenuNodeValueの各型の値取得
 */
bool	FCSDebugMenuNodeValue::GetBool() const
{
	return mValueString.ToBool();
}
int32	FCSDebugMenuNodeValue::GetInt() const
{
	return FCString::Atoi(*mValueString);
}
float	FCSDebugMenuNodeValue::GetFloat() const
{
	return FCString::Atof(*mValueString);
}
bool	FCSDebugMenuNodeValue::GetSelectorList(FCSDebugMenuNodeValueKindSelector& OutList) const
{
	return OutList.FromJson(mValueString);
}

/**
 * @brief	FCSDebugMenuNodeValueの各型の値設定
 */
void	FCSDebugMenuNodeValue::Set(const bool bInBool)
{
	mValueString = bInBool ? TEXT("true") : TEXT("false");
	mKind = (int32)ECSDebugMenuNodeValueKind::Bool;

	CallDelegate();
}
void	FCSDebugMenuNodeValue::Set(const int32 InInt)
{
	mValueString = FString::FromInt(InInt);
	mKind = (int32)ECSDebugMenuNodeValueKind::Int;

	CallDelegate();
}
void	FCSDebugMenuNodeValue::Set(const float InFloat)
{
	mValueString = FString::SanitizeFloat(InFloat);
	mKind = (int32)ECSDebugMenuNodeValueKind::Float;

	CallDelegate();
}
void	FCSDebugMenuNodeValue::Set(const int32 InListIndex, const TArray<FString>& InList)
{
	FCSDebugMenuNodeValueKindSelector ListData;
	ListData.mIndex = InListIndex;
	ListData.mList = InList;
	mValueString = ListData.ToJson();
	mKind = (int32)ECSDebugMenuNodeValueKind::Selector;

	CallDelegate();
}
void	FCSDebugMenuNodeValue::SetSelectorListIndex(const int32 InListIndex)
{
	FCSDebugMenuNodeValueKindSelector ListData;
	if (GetSelectorList(ListData))
	{
		ListData.mIndex = InListIndex;
		mValueString = ListData.ToJson();
	}
}
void	FCSDebugMenuNodeValue::Set(const FCSDebugMenuNodeDelegate& InDelegate)
{
	mKind = (int32)ECSDebugMenuNodeValueKind::Button;
	mDelegate = InDelegate;
}
void	FCSDebugMenuNodeValue::AddMenuList(const FCSDebugMenuNodeValue& InNodeValue)
{
	check(mKind == (int32)ECSDebugMenuNodeValueKind::Invalid || mKind == (int32)ECSDebugMenuNodeValueKind::Menu);
	if (FCSDebugMenuNodeValue* Node = FindCurrentNode(InNodeValue.GetDispString()))
	{
		Node->SetRuntimeNode(InNodeValue);
		Node->mbAddRuntime = true;
	}
	else
	{
		mList.Add(InNodeValue);
	}
	mKind = (int32)ECSDebugMenuNodeValueKind::Menu;
}
FCSDebugMenuNodeValue*	FCSDebugMenuNodeValue::GetCurrentLastNode()
{
	const int32 ListNum = mList.Num();
	if (ListNum > 0)
	{
		return &mList[ListNum - 1];
	}
	return nullptr;
}
FCSDebugMenuNodeValue* FCSDebugMenuNodeValue::FindCurrentNode(const FString& InDispString)
{
	for (FCSDebugMenuNodeValue& Node : mList)
	{
		if (Node.GetDispString() == InDispString)
		{
			return &Node;
		}
	}
	return nullptr;
}
/**
 * @brief	指定パスのNode取得
 */
FCSDebugMenuNodeValue* FCSDebugMenuNodeValue::FindAllNode(const FString& InPath)
{
	TArray<FString> MenuPathSplit;
	InPath.ParseIntoArray(MenuPathSplit, TEXT("/"));
	if (MenuPathSplit.Num() == 0)
	{
		return this;
	}

	FCSDebugMenuNodeValue* FindNode = this;
	while (MenuPathSplit.Num() > 0)
	{
		bool bFind = false;
		for (FCSDebugMenuNodeValue& Node : FindNode->mList)
		{
			if (Node.mDispString == MenuPathSplit[0])
			{
				FindNode = &Node;
				MenuPathSplit.RemoveAt(0);
				bFind = true;
				break;
			}
		}

		if (!bFind)
		{
			return nullptr;
		}
	}
	return FindNode;
}
const FCSDebugMenuNodeValue* FCSDebugMenuNodeValue::FindAllNode(const FString& InPath) const
{
	TArray<FString> MenuPathSplit;
	InPath.ParseIntoArray(MenuPathSplit, TEXT("/"));
	if (MenuPathSplit.Num() == 0)
	{
		return this;
	}

	const FCSDebugMenuNodeValue* FindNode = this;
	while (MenuPathSplit.Num() > 0)
	{
		bool bFind = false;
		for (const FCSDebugMenuNodeValue& Node : FindNode->mList)
		{
			if (Node.mDispString == MenuPathSplit[0])
			{
				FindNode = &Node;
				MenuPathSplit.RemoveAt(0);
				bFind = true;
				break;
			}
		}

		if (!bFind)
		{
			return nullptr;
		}
	}
	return FindNode;
}

/**
 * @brief	無効なNodeを削除
 */
void	FCSDebugMenuNodeValue::CheckDeleteNode()
{
	int32 Index = 0;
	while(Index < mList.Num())
	{
		FCSDebugMenuNodeValue& Node = mList[Index];
		if (!Node.mbFromEUW
			&& !Node.mbAddRuntime)
		{
			mList.RemoveAt(Index);
			continue;
		}
		if (Node.GetKind() == ECSDebugMenuNodeValueKind::Menu)
		{
			Node.CheckDeleteNode();//menuの階層が深いと再帰は危ないかも
		}
		++Index;
	}
}

/**
 * @brief	Delegate実行
 */
void	FCSDebugMenuNodeValue::CallDelegate()
{
	const FCSDebugMenuNodeGetter Getter(*this);
	mDelegate.ExecuteIfBound(Getter);
}