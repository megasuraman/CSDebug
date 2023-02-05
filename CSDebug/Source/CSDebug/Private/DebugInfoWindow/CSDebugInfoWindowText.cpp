// Copyright 2020 SensyuGames.
/**
 * @file CSDebugInfoWindowText.cpp
 * @brief デバッグ情報表示用Window　テキスト表示
 * @author SensyuGames
 * @date 2021/12/27
 */


#include "DebugInfoWindow/CSDebugInfoWindowText.h"


#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "CanvasItem.h"


 /**
  * @brief WindowSizeを表示テキストに合わせる
  */
void	FCSDebugInfoWindowText::FittingWindowExtent(class UCanvas* InCanvas)
{
	const float WidthSpaceLen = mWidthInterval;
	FVector2D Extent = FVector2D::ZeroVector;
	Extent.Y += mHeightInterval;
	for (int32 i = 0; i < mStringList.GetListNum(); ++i)
	{
		const FString& String = mStringList.GetOrder(i);
		float StringWidth = 0.f;
		float StringHeight = 0.f;
		CalcTextDispWidthHeight(StringWidth, StringHeight, InCanvas, String);
		if (Extent.X < StringWidth + WidthSpaceLen)
		{
			Extent.X = StringWidth + WidthSpaceLen;
		}
		Extent.Y += StringHeight + mHeightInterval;
	}
	Extent.Y += mHeightInterval;
	SetWindowExtent(Extent);
}

/**
 * @brief 表示文字列追加
 */
void	FCSDebugInfoWindowText::AddText(const FString& InString)
{
	FVector2D WindowExtent = GetWindowExtent();
	TArray<FString> LineString;
	InString.ParseIntoArray(LineString, TEXT("\n"));
	for (const FString& String : LineString)
	{
		mStringList.Push(String);
		WindowExtent.Y += mFontHeight + mHeightInterval * 2.f;
		const float StringWidth = mWidthInterval + (String.Len() * mFontWidth);
		WindowExtent.X = FMath::Max(WindowExtent.X, StringWidth);
	}

	SetWindowExtent(WindowExtent);
}

/**
 * @brief Windowの下敷き表示後処理
 */
void	FCSDebugInfoWindowText::DrawAfterBackground(UCanvas* InCanvas, const FVector2D& InPos2D) const
{
	FVector2D StringPos = InPos2D;
	StringPos.X += mWidthInterval;//ちょっと隙間空けたい

	for(int32 i=0; i<mStringList.GetListNum(); ++i)
	{
		const FString& String = mStringList.GetOrder(i);
		StringPos.Y += mHeightInterval;

		FCanvasTextItem Item(StringPos, FText::FromString(String), GEngine->GetSmallFont(), mFontColor);
		Item.Scale = FVector2D(1.f);
		InCanvas->DrawItem(Item);

		StringPos.Y += mFontHeight;
	}
}