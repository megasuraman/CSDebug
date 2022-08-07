// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMath.h
 * @brief �v�Z�֐����̂܂Ƃ�
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
	   !3�����̒P���ȕ�Ԋ֐�(FMath::InterpEaseInOut<float>(0, 1, InRatio, 2.0)�ɋ߂�)
	------------------------------------------------------------ */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "CSDebugMath")
	static float CalcSimpleEaseInOut(float InRatio)
	{
		InRatio = FMath::Clamp(InRatio, 0.f, 1.f);
		return (3.f - 2.f * InRatio) * FMath::Square(InRatio);
	}
	/* ------------------------------------------------------------
	   !PosA-PosB, PosB-PosC�ɂ��2���x�W�F�Ȑ��̍��W�擾(PosB������_)
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
