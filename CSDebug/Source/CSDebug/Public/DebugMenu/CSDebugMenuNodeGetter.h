// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMenuNodeGetter.h
 * @brief CSDebugMenu�̊O���Q�ƗpNode���
 * @author SensyuGames
 * @date 2020/08/01
 */

#pragma once

#include "CoreMinimal.h"
#include "CSDebugMenuNodeData.h"
#include "CSDebugMenuNodeGetter.generated.h"

struct FCSDebugMenuNodeValue;

// �O���Q�Ɨp�m�[�h���(FCSDebugMenuNodeValue�͕�Module���猩���Ȃ��̂�)
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


// EUW�p��FCSDebugMenuNodeValue��Blueprint��
// �ċA�I��TArray�̓T�|�[�g����ĂȂ��̂�TArray<FCSDebugMenuNodeValue>�͍폜
USTRUCT(BlueprintType)
struct CSDEBUG_API FCSDebugMenuNodeEUW
{
	GENERATED_USTRUCT_BODY()

public:
	FCSDebugMenuNodeEUW() {}
	FCSDebugMenuNodeEUW(const FCSDebugMenuNodeValue& InValue);
	
	UPROPERTY(BlueprintReadWrite)
	FString mOriginJsonString;//TArray<FCSDebugMenuNodeValue>�̑���
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