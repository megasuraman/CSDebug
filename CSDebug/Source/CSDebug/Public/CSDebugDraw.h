// Copyright 2020 SensyuGames.
/**
 * @file CSDebugDraw.h
 * @brief Debug表示用のまとめクラス
 * @author SensyuGames
 * @date 2022/5/12
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebugDraw.generated.h"

class UCanvas;
class AAIController;

/**
 * 
 */
UCLASS()
class CSDEBUG_API UCSDebugDraw : public UObject
{
	GENERATED_BODY()
	
public:
	static void DrawOctahedronArrow(UWorld* InWorld, const FVector& InBasePos, const FVector& InTargetPos, const float InRadius, const FColor InColor, float InCenterRatio=0.25f, float InLifeTime = -1.f, const uint8 InDepthPriority = 0, const float InThickness = 0.f);
	
	static void DrawPathFollowRoute(UWorld* InWorld, UCanvas* InCanvas, const AAIController* InAIController, const bool bInShowDetail);
	static void DrawLastEQS(UWorld* InWorld, UCanvas* InCanvas, const AAIController* InAIController, const float InShowDetailDistance=500.f);

	static void DrawCanvasQuadrangle(UCanvas* InCanvas, const FVector2D& InCenterPos, const FVector2D& InExtent, const FLinearColor InColor);
	static void DrawCanvasQuadrangle(UCanvas* InCanvas, const FVector& InPos, const FVector2D& InExtent, const FLinearColor InColor);
};
