// Copyright 2020 SensyuGames.
/**
 * @file CSDebugEUW_DTUID.h
 * @brief DataTableñàÇ…ÉÜÉjÅ[ÉNÇ»EnumÇê∂ê¨Ç∑ÇÈ
 * @author SensyuGames
 * @date 2022/12/27
 */

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "CSDebugEUW_DTUID.generated.h"

class UDataTable;
/**
 * 
 */
UCLASS()
class CSDEBUGEDITOR_API UCSDebugEUW_DTUID : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void	GenerateDataTableUID(const UDataTable* InDataTable);

protected:
	struct FDTUIDInfo
	{
		FString	mName;
		uint32	mId = 0;
		bool	mbDelete = false;
	};
	bool	GetExistDTUID(TArray<FDTUIDInfo>& OutDTUIDList, const FString& InPath) const;
	void	CheckDataTableRow(TArray<FDTUIDInfo>& OutDTUIDList, const UDataTable* InDataTable) const;
	bool	CreateStringRelacepMap(TMap<FString, FString>& OutStringReplaceMap, const TArray<FDTUIDInfo>& InDTUIDList, const FString& InDataTableName) const;
	bool	GetNewFileString(FString& OutFileString, const FString& InBaseFilePath, const TMap<FString, FString>& InStringReplaceMap);
};
