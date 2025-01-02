// Copyright 2022 SensyuGames.

#include "DebugMenu/CSDebug_DebugMenuNodeButton.h"


void CSDebug_DebugMenuNodeButton::OnEndAction(const FCSDebug_DebugMenuNodeActionParameter& InParameter)
{
	CSDebug_DebugMenuNodeBase::OnEndAction(InParameter);
}

void CSDebug_DebugMenuNodeButton::DrawValue(UCanvas* InCanvas, const FVector2D& InPos, const FLinearColor InColor) const
{
	FCanvasTextItem Item(InPos, FText::FromString(FString(TEXT("実行"))), GEngine->GetSmallFont(), InColor);
	Item.Scale = FVector2D(1.f);
	InCanvas->DrawItem(Item);
}
