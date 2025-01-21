// Copyright 2022 SensyuGames.


#include "DebugMenu/CSDebug_DebugMenuNodeBase.h"
#include "DebugMenu/CSDebug_DebugMenuManager.h"


CSDebug_DebugMenuNodeBase::CSDebug_DebugMenuNodeBase()
{
}

CSDebug_DebugMenuNodeBase::~CSDebug_DebugMenuNodeBase()
{
}

void CSDebug_DebugMenuNodeBase::Init(const FString& InPath, const FCSDebug_DebugMenuNodeData& InData, UCSDebug_DebugMenuManager* InManager)
{
	mNodeData = InData;
	if (mNodeData.mKind == ECSDebug_DebugMenuValueKind::Folder)
	{
		mPath = InPath;
	}
	else
	{
		mPath = FString::Printf(TEXT("%s/%s"), *InPath, *mNodeData.mDisplayName);
	}
	mManager = InManager;
	switch (mNodeData.mKind)
	{
	case ECSDebug_DebugMenuValueKind::Bool:
		SetValueBool(false);
		SetInitValue();
		break;
	case ECSDebug_DebugMenuValueKind::Int:
		SetValueInt(0);
		SetInitValue();
		break;
	case ECSDebug_DebugMenuValueKind::Float:
		SetValueFloat(0.f);
		SetInitValue();
		break;
	case ECSDebug_DebugMenuValueKind::List:
		SetValueList(0);
		SetInitValue();
		break;
	case ECSDebug_DebugMenuValueKind::Enum:
	{
		mNodeData.mList.Empty();
		if(const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *mNodeData.mEnumName, true))
		{
			for (int32 i = 0; i < EnumPtr->NumEnums() - 1; ++i)
			{
				FString EnumName = EnumPtr->GetNameStringByIndex(i);
				mNodeData.mList.Add(EnumName);
			}
		}
		SetValueList(0);
		SetInitValue();
		break;
	}
	default:
		break;
	}
}

void CSDebug_DebugMenuNodeBase::OnBeginAction()
{
	mbEditMode = true;
}

void CSDebug_DebugMenuNodeBase::OnEndAction(const FCSDebug_DebugMenuNodeActionParameter& InParameter)
{
	mActionDelegate.ExecuteIfBound(InParameter);
	mbEditMode = false;
}

void CSDebug_DebugMenuNodeBase::Draw(UCanvas* InCanvas, const FVector2D& InPos, const bool bInSelect) const
{
	FVector2D WindowExtent(300.f, 20.f);
	const float ValueLineOffsetX = 200.f;
	const bool bFolderNode = mNodeData.mKind == ECSDebug_DebugMenuValueKind::Folder;
	if (bFolderNode)
	{
		WindowExtent.X = ValueLineOffsetX;
	}
	const FVector2D StringOffset(2.f, 2.f);
	const FLinearColor WindowBackColor = GetWindowBackColor();
	const FLinearColor WindowFrameColor = GetWindowFrameColor();
	const FLinearColor FontColor = GetFontColor();
	// 下敷き
	{
		FCanvasTileItem Item(InPos, WindowExtent, WindowBackColor);
		Item.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;
		InCanvas->DrawItem(Item);
	}
	// 枠
	{
		FCanvasBoxItem Item(InPos, WindowExtent);
		if (bInSelect
			&& !mbEditMode)
		{
			Item.SetColor(GetSelectColor());
			Item.LineThickness = 3.f;
		}
		else
		{
			Item.SetColor(WindowFrameColor);
			Item.LineThickness = 1.f;
		}
		InCanvas->DrawItem(Item);
	}
	// 項目名表示
	{
		const FVector2D StringPos = InPos + StringOffset;
		FCanvasTextItem Item(StringPos, FText::FromString(mNodeData.mDisplayName), GEngine->GetSmallFont(), FontColor);
		Item.Scale = FVector2D(1.f);
		InCanvas->DrawItem(Item);
	}
	if(!bFolderNode)
	{
		// 値表示線
		const FVector2D BorderLinePosBegin(InPos.X + ValueLineOffsetX, InPos.Y);
		const FVector2D BorderLinePosEnd(InPos.X + ValueLineOffsetX, InPos.Y + WindowExtent.Y);
		DrawDebugCanvas2DLine(InCanvas, BorderLinePosBegin, BorderLinePosEnd, WindowFrameColor);

		// 値表示
		const FVector2D StringPos = BorderLinePosBegin + FVector2D(2.f, 2.f);
		DrawValue(InCanvas, StringPos, FontColor);

		if (mbEditMode)
		{
			const FVector2D ValueFrameExtent(WindowExtent.X - ValueLineOffsetX, WindowExtent.Y);
			DrawEditValue(InCanvas, BorderLinePosBegin, ValueFrameExtent);
		}
	}
}

FString CSDebug_DebugMenuNodeBase::GetDrawValueString() const
{
	if (mNodeData.mKind == ECSDebug_DebugMenuValueKind::List
		|| mNodeData.mKind == ECSDebug_DebugMenuValueKind::Enum)
	{
		return GetSelectString();
	}
	return mValueString;
}

bool CSDebug_DebugMenuNodeBase::GetBool() const
{
	ensure(mNodeData.mKind == ECSDebug_DebugMenuValueKind::Bool);
	return mValueString.ToBool();
}

int32 CSDebug_DebugMenuNodeBase::GetInt() const
{
	ensure(mNodeData.mKind == ECSDebug_DebugMenuValueKind::Int);
	return FCString::Atoi(*mValueString);
}

float CSDebug_DebugMenuNodeBase::GetFloat() const
{
	ensure(mNodeData.mKind == ECSDebug_DebugMenuValueKind::Float);
	return FCString::Atof(*mValueString);
}

int32 CSDebug_DebugMenuNodeBase::GetSelectIndex() const
{
	ensure(mNodeData.mKind == ECSDebug_DebugMenuValueKind::List
			|| mNodeData.mKind == ECSDebug_DebugMenuValueKind::Enum);
	return FCString::Atoi(*mValueString);
}

FString CSDebug_DebugMenuNodeBase::GetSelectString() const
{
	ensure(mNodeData.mKind == ECSDebug_DebugMenuValueKind::List
		|| mNodeData.mKind == ECSDebug_DebugMenuValueKind::Enum);
	const int32 SelectIndex = GetSelectIndex();
	if (SelectIndex < mNodeData.mList.Num())
	{
		return mNodeData.mList[SelectIndex];
	}
	return FString();
}

void CSDebug_DebugMenuNodeBase::SetNodeAction(const FCSDebug_DebugMenuNodeActionDelegate& InDelegate)
{
	if (mActionDelegate.IsBound())
	{
		UE_LOG(CSDebugLog, Error, TEXT("Delegate is already set"));
		return;
	}
	mActionDelegate = InDelegate;
}

void CSDebug_DebugMenuNodeBase::Load(const FString& InValueString)
{
	SetValueString(InValueString);
}

void CSDebug_DebugMenuNodeBase::SetInitValue()
{
	if (!mNodeData.mInitValue.IsEmpty())
	{
		SetValueString(mNodeData.mInitValue);
	}
}

void CSDebug_DebugMenuNodeBase::SetValueBool(const bool InValue)
{
	ensure(mNodeData.mKind == ECSDebug_DebugMenuValueKind::Bool);
	mValueString = InValue ? TEXT("true") : TEXT("false");
}

void CSDebug_DebugMenuNodeBase::SetValueInt(const int32 InValue)
{
	ensure(mNodeData.mKind == ECSDebug_DebugMenuValueKind::Int);
	mValueString = FString::FromInt(InValue);
}

void CSDebug_DebugMenuNodeBase::SetValueFloat(const float InValue)
{
	ensure(mNodeData.mKind == ECSDebug_DebugMenuValueKind::Float);
	mValueString = FString::SanitizeFloat(InValue);
}

void CSDebug_DebugMenuNodeBase::SetValueList(const int32 InSelectIndex)
{
	ensure(mNodeData.mKind == ECSDebug_DebugMenuValueKind::List
			|| mNodeData.mKind == ECSDebug_DebugMenuValueKind::Enum);
	if (InSelectIndex < mNodeData.mList.Num())
	{
		mValueString = FString::FromInt(InSelectIndex);
	}
}

void CSDebug_DebugMenuNodeBase::DrawValue(UCanvas* InCanvas, const FVector2D& InPos, const FLinearColor InColor) const
{
	FCanvasTextItem Item(InPos, FText::FromString(GetDrawValueString()), GEngine->GetSmallFont(), InColor);
	Item.Scale = FVector2D(1.f);
	InCanvas->DrawItem(Item);
}

void CSDebug_DebugMenuNodeBase::DrawEditValue(UCanvas* InCanvas, const FVector2D& InValuePos, const FVector2D& InValueExtent) const
{
	FCanvasBoxItem Item(InValuePos, InValueExtent);
	Item.SetColor(GetSelectColor());
	Item.LineThickness = 3.f;
	InCanvas->DrawItem(Item);
}

UCSDebug_DebugMenuManager* CSDebug_DebugMenuNodeBase::GetManager() const
{
	return mManager.Get();
}