// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMenuNodeGetter.h
 * @brief CSDebugMenuの外部参照用Node情報
 * @author SensyuGames
 * @date 2020/08/01
 */

#pragma once

#include "CoreMinimal.h"
#include "CSDebugMenuNodeData.h"
#include "CSDebugMenuNodeGetter.generated.h"

struct FCSDebugMenuNodeValue;

// 外部参照用ノード情報(FCSDebugMenuNodeValueは別Moduleから見えないので)
USTRUCT()
struct CSDEBUG_API FCSDebugMenuNodeGetter
{
	GENERATED_USTRUCT_BODY()

public:
	FCSDebugMenuNodeGetter() {}
	FCSDebugMenuNodeGetter(const FCSDebugMenuNodeValue& InValue);

	bool	GetBool() const;
	int32	GetInt() const;
	float	GetFloat() const;
	int32	GetSelectIndex() const;
	FString	GetSelectString() const;

private:
	FString mValueString;
};


// EUW用にFCSDebugMenuNodeValueのBlueprint版
// 再帰的なTArrayはサポートされてないのでTArray<FCSDebugMenuNodeValue>は削除
USTRUCT(BlueprintType)
struct CSDEBUG_API FCSDebugMenuNodeEUW
{
	GENERATED_USTRUCT_BODY()

public:
	FCSDebugMenuNodeEUW() {}
	FCSDebugMenuNodeEUW(const FCSDebugMenuNodeValue& InValue);
	
	UPROPERTY(BlueprintReadWrite)
	FString mOriginJsonString;//TArray<FCSDebugMenuNodeValue>の代わり
	UPROPERTY(BlueprintReadWrite)
	FString mDispString;
	UPROPERTY(BlueprintReadWrite)
	FString mValueString;
	UPROPERTY(BlueprintReadWrite)
	int32	mKind = 0;
	UPROPERTY(BlueprintReadWrite)
	bool	mbFromEUW = false;

	void	GetNodeValue(FCSDebugMenuNodeValue& OutNode) const;
};