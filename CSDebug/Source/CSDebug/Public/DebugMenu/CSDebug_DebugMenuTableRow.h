// Copyright 2022 SensyuGames.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CSDebug_DebugMenuTableRow.generated.h"

UENUM(BlueprintType)
enum class ECSDebug_DebugMenuValueKind : uint8
{
	Invalid,
	Bool,
	Int,
	Float,
	List,
	Enum,
	Button,
	Folder UMETA(Hidden),
};

USTRUCT(BlueprintType)
struct CSDEBUG_API FCSDebug_DebugMenuNodeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "表示名", DisplayPriority = 1))
	FString mDisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "コメント", DisplayPriority = 1, MultiLine = true))
	FString mComment;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Enum型名", DisplayPriority = 4, EditCondition = "mKind == ECSDebug_DebugMenuValueKind::Enum"))
	FString mEnumName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "初期値", DisplayPriority = 3))
	FString mInitValue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "文字列リスト", DisplayPriority = 4, EditCondition = "mKind == ECSDebug_DebugMenuValueKind::List"))
	TArray<FString> mList;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "型", DisplayPriority = 2))
	ECSDebug_DebugMenuValueKind mKind = ECSDebug_DebugMenuValueKind::Invalid;
};

USTRUCT(BlueprintType)
struct CSDEBUG_API FCSDebug_DebugMenuTableRow : public FTableRowBase
{
	GENERATED_BODY()

// 	UPROPERTY(VisibleAnywhere, meta = (DisplayName = "フォルダパス", DisplayPriority = 1))
// 	FString mDisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "項目リスト", DisplayPriority = 1, TitleProperty = "mDisplayName"))
	TArray<FCSDebug_DebugMenuNodeData> mNodeList;
};
