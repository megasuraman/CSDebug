// Copyright 2022 SensyuGames.

#include "DebugMenu/CSDebug_DebugMenuNodeFolder.h"
#include "DebugMenu/CSDebug_DebugMenuManager.h"


void CSDebug_DebugMenuNodeFolder::OnEndAction(const FCSDebug_DebugMenuNodeActionParameter& InParameter)
{
	CSDebug_DebugMenuNodeBase::OnEndAction(InParameter);
	if (UCSDebug_DebugMenuManager* Manager = GetManager())
	{
		Manager->SetMainFolder(GetPath());
	}
}

void CSDebug_DebugMenuNodeFolder::OnJustPressedRightKey()
{
	if (UCSDebug_DebugMenuManager* Manager = GetManager())
	{
		Manager->SetMainFolder(GetPath());
	}
}

void CSDebug_DebugMenuNodeFolder::DrawValue(UCanvas* InCanvas, const FVector2D& InPos, const FLinearColor InColor) const
{
	FCanvasTextItem Item(InPos, FText::FromString(FString(TEXT("フォルダ移動"))), GEngine->GetSmallFont(), InColor);
	Item.Scale = FVector2D(1.f);
	InCanvas->DrawItem(Item);
}
