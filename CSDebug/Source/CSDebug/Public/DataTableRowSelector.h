// Copyright 2022 SensyuGames.

#pragma once

#include "CoreMinimal.h"
#include "DataTableRowSelector.generated.h"

USTRUCT(BlueprintType)
struct FDataTableRowSelector
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTableRowSelector")
	FName mRowName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTableRowSelector")
	FString mDataTablePath;

	FDataTableRowSelector() {}
	FDataTableRowSelector(const FString& InDataTablePath)
	{
		mDataTablePath = InDataTablePath;
	}
};
