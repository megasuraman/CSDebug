// Copyright 2020 SensyuGames.
/**
 * @file CSDebugInfoWindowBase.cpp
 * @brief デバッグ情報表示用WindowBase
 * @author SensyuGames
 * @date 2021/12/27
 */


#include "DebugInfoWindow/CSDebugInfoWindowBase.h"


#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "CanvasItem.h"
#include "DrawDebugHelpers.h"
#include "RenderUtils.h"


 /**
  * @brief Window表示(2D座標)
  */
void	FCSDebugInfoWindowBase::Draw(UCanvas* InCanvas, const FVector2D& InPos2D) const
{
	if (mWindowExtent.IsZero())
	{
		return;
	}
	float WindowNameWidth = 0.f;
	if (mWindowName.Len() > 0)
	{
		WindowNameWidth = DrawWindowName(InCanvas, InPos2D);
	}

	FVector2D WindowExtent = mWindowExtent;
	WindowExtent.X = FMath::Max(WindowExtent.X, WindowNameWidth);
	// 下敷き
	{
		FCanvasTileItem Item(InPos2D, WindowExtent, mWindowBackColor);
		Item.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;
		InCanvas->DrawItem(Item);
	}
	// 枠
	{
		FCanvasBoxItem Item(InPos2D, WindowExtent);
		Item.SetColor(mWindowFrameColor);
		InCanvas->DrawItem(Item);
	}

	DrawAfterBackground(InCanvas, InPos2D);

#if 0//CalcTextDispWidthHeight()のテスト
	FVector2D TestPos(100.f,100.f);
	FString TestString(TEXT("abcdefghijk"));
	float StringWidth = 0.f;
	float StringHeight = 0.f;
	CalcTextDispWidthHeight(StringWidth, StringHeight, InCanvas, TestString);

	DrawDebugCanvas2DLine(InCanvas, TestPos, TestPos + FVector2D(StringWidth, 0.f), FColor::Blue);
	DrawDebugCanvas2DLine(InCanvas, TestPos, TestPos + FVector2D(0.f, StringHeight), FColor::Blue);
	DrawDebugCanvas2DLine(InCanvas, TestPos + FVector2D(StringWidth, 0.f), TestPos + FVector2D(StringWidth, StringHeight), FColor::Blue);

	FCanvasTextItem TextItem(TestPos, FText::FromString(TestString), GetUseFont(), mWindowNameColor);
	TextItem.Scale = FVector2D(mFontScale);
	InCanvas->DrawItem(TextItem);
#endif
}

/**
 * @brief Window表示(Windowサイズ比での2D座標)
 */
void	FCSDebugInfoWindowBase::Draw(UCanvas* InCanvas, const float InPosRatioX, const float InPosRatioY) const
{
	FVector2D ScreenPos(InCanvas->ClipX * InPosRatioX, InCanvas->ClipY * InPosRatioY);
	Draw(InCanvas, ScreenPos);
}

/**
 * @brief Window表示(3D座標指定)
 */
void	FCSDebugInfoWindowBase::Draw(UCanvas* InCanvas, const FVector& InPos, const float InBorderDistance) const
{
	const FVector ProjectPos = InCanvas->Project(InPos);
	if (ProjectPos.X < 0.f
		|| ProjectPos.X > InCanvas->SizeX
		|| ProjectPos.Y < 0.f
		|| ProjectPos.Y > InCanvas->SizeY
		|| ProjectPos.Z <= 0.f)
	{
		return;
	}

	const FVector2D ScreenPos(ProjectPos);
	Draw(InCanvas, ScreenPos);
}

/**
 * @brief Window名部分表示
 */
float	FCSDebugInfoWindowBase::DrawWindowName(UCanvas* InCanvas, const FVector2D& InPos2D) const
{
	const float WindowInsideOffset = 6.f;
	const float WindowWidthSpace = 4.f;
	const float WindowHeightSpace = 2.f;
	float BaseWindowWidth = 0.f;
	float BaseWindowHeight = 0.f;
	CalcTextDispWidthHeight(BaseWindowWidth, BaseWindowHeight, InCanvas, mWindowName);
	BaseWindowWidth += WindowInsideOffset * 2.f + WindowWidthSpace*2.f;
	BaseWindowHeight += WindowHeightSpace * 2.f;

	FVector2D WindowEdgePos = InPos2D;
	WindowEdgePos.Y -= BaseWindowHeight;
	const uint32 WindowPointListSize = 4;
	const FVector2D WindowPointList[WindowPointListSize] = {
		FVector2D(WindowEdgePos.X + WindowInsideOffset, WindowEdgePos.Y),//左上
		FVector2D(WindowEdgePos.X, WindowEdgePos.Y + BaseWindowHeight),//左下
		FVector2D(WindowEdgePos.X + BaseWindowWidth, WindowEdgePos.Y + BaseWindowHeight),//右下
		FVector2D(WindowEdgePos.X + BaseWindowWidth - WindowInsideOffset, WindowEdgePos.Y)//右上
	};
	for (uint32 i = 0; i < WindowPointListSize; ++i)
	{//枠
		DrawDebugCanvas2DLine(InCanvas, WindowPointList[i], WindowPointList[(i + 1) % WindowPointListSize], mWindowFrameColor);
	}
	{//下敷き
		{
			FCanvasTriangleItem TileItem(WindowPointList[0], WindowPointList[1], WindowPointList[2], GWhiteTexture);
			TileItem.SetColor(mWindowBackColor);
			TileItem.BlendMode = SE_BLEND_Translucent;
			InCanvas->DrawItem(TileItem);
		}
		{
			FCanvasTriangleItem TileItem(WindowPointList[2], WindowPointList[3], WindowPointList[0], GWhiteTexture);
			TileItem.SetColor(mWindowBackColor);
			TileItem.BlendMode = SE_BLEND_Translucent;
			InCanvas->DrawItem(TileItem);
		}
	}
	FVector2D TextPos = WindowEdgePos;
	TextPos.X += WindowInsideOffset + WindowWidthSpace;
	TextPos.Y += WindowHeightSpace;
	FCanvasTextItem TextItem(TextPos, FText::FromString(mWindowName), GetUseFont(), mWindowNameColor);
	TextItem.Scale = FVector2D(mFontScale);
	InCanvas->DrawItem(TextItem);

	return BaseWindowWidth;
}
/**
 * @brief	使用フォント取得
 * @param
 * @return
 */
UFont* FCSDebugInfoWindowBase::GetUseFont() const
{
	return GEngine->GetMediumFont();
}
/**
 * @brief	テキスト描画時の幅と高さを取得
 * @param
 * @return
 */
void	FCSDebugInfoWindowBase::CalcTextDispWidthHeight(float& OutWidth, float& OutHeight, UCanvas* InCanvas, const FString& InText) const
{
	//InCanvas->TextSize(GetUseFont(), InText, OutWidth, OutHeight, mFontScale, mFontScale);
	InCanvas->StrLen(GetUseFont(), InText, OutWidth, OutHeight, true);
	//OutWidth *= 1.15f;//何故かズレる大きめに適当な調整(4.25だと変？)
}