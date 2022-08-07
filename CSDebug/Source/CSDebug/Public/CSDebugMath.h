// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMath.h
 * @brief 計算関数等のまとめ
 * @author SensyuGames
 * @date 2022/3/19
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebugMath.generated.h"

class APawn;
struct FEnvQueryInstance;

/**
 * 
 */
UCLASS()
class CSDEBUG_API UCSDebugMath : public UObject
{
	GENERATED_BODY()
	
public:
	/* ------------------------------------------------------------
	   !3次式の単純な補間関数(FMath::InterpEaseInOut<float>(0, 1, InRatio, 2.0)に近い)
	------------------------------------------------------------ */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "CSDebugMath")
	static float CalcSimpleEaseInOut(float InRatio)
	{
		InRatio = FMath::Clamp(InRatio, 0.f, 1.f);
		return (3.f - 2.f * InRatio) * FMath::Square(InRatio);
	}
	/* ------------------------------------------------------------
	   !PosA-PosB, PosB-PosCによる2次ベジェ曲線の座標取得(PosBが制御点)
	------------------------------------------------------------ */
	static FVector CalcQuadraticBezier(const FVector& InPosA, const FVector& InPosB, const FVector& InPosC, float InRatio)
	{
		const FVector PosAB = FMath::Lerp(InPosA, InPosB, InRatio);
		const FVector PosBC = FMath::Lerp(InPosB, InPosC, InRatio);
		const FVector PosABBC = FMath::Lerp(PosAB, PosBC, InRatio);
		return PosABBC;
	}

	static FEnvQueryInstance* FindLastEnvQueryInstance(float& OutLastTimeStamp, const APawn* InOwner);
};
