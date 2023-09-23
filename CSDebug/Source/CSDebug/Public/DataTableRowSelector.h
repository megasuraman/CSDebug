// Copyright 2022 SensyuGames.

#pragma once

#include "CoreMinimal.h"
#include "DataTableRowSelector.generated.h"

USTRUCT(BlueprintType)
struct FDataTableRowSelector
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "DataTableRowSelector")
	FName mRowName;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "DataTableRowSelector")
	FString mDataTablePath;
	UPROPERTY(EditAnywhere, Category = "DataTableRowSelector")
	UStruct* mDataTableStruct;
	UPROPERTY(EditAnywhere, Category = "DataTableRowSelector")
	FString mDisplayName;
#endif

	FDataTableRowSelector() {}
	FDataTableRowSelector(const FString& InDataTablePath)
	{
		mDataTablePath = InDataTablePath;
	}
};
