// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_Utility.cpp
 * @brief 便利関数等のまとめ(分類しにくかった奴)
 * @author SensyuGames
 * @date 2023/02/24
 */
#include "CSDebug_Utility.h"

#include "EnvironmentQuery/EnvQueryManager.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#if WITH_EDITOR
#include "Editor.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#endif

/**
 * @brief	Enumの要素名取得
 */
FString UCSDebug_Utility::GetUEnumString(const TCHAR* InEnumTypeName, int32 InEnumValue)
{
	if (const UEnum* EnumPointer = FindObject< UEnum >(ANY_PACKAGE, InEnumTypeName, true))
	{
		return EnumPointer->GetNameStringByValue((int64)InEnumValue);
	}
	return FString::Printf(TEXT("%d"), InEnumValue);
}

/**
 * @brief	最終のEQS情報取得
 */
FEnvQueryInstance* UCSDebug_Utility::FindLastEnvQueryInstance(float& OutLastTimeStamp, const APawn* InOwner)
{
	if (InOwner == nullptr)
	{
		return nullptr;
	}
#if USE_EQS_DEBUGGER
	UWorld* World = InOwner->GetWorld();
	UEnvQueryManager* QueryManager = UEnvQueryManager::GetCurrent(World);
	if (QueryManager == nullptr)
	{
		return nullptr;
	}

	const TArray<FEQSDebugger::FEnvQueryInfo>& QueryDataList = QueryManager->GetDebugger().GetAllQueriesForOwner(InOwner);
	int32 LastQueryDataIndex = INDEX_NONE;
	float MaxTimeStamp = 0.f;
	for (int32 i = 0; i < QueryDataList.Num(); ++i)
	{
		const FEQSDebugger::FEnvQueryInfo& Info = QueryDataList[i];
		if (Info.Timestamp > MaxTimeStamp)
		{
			MaxTimeStamp = Info.Timestamp;
			LastQueryDataIndex = i;
		}
	}

	if (LastQueryDataIndex == INDEX_NONE)
	{
		return nullptr;
	}

	OutLastTimeStamp = MaxTimeStamp;
	return QueryDataList[LastQueryDataIndex].Instance.Get();
#else
	return nullptr;
#endif
}

/**
 * @brief	プレイヤーキャラクターを非表示状態で指定Actorへ憑依させる
 */
void UCSDebug_Utility::MakeGhostPlayer(ACharacter* InPlayer, AActor* InTarget)
{
	FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, true);
	InPlayer->AttachToActor(InTarget, Rules);

	InPlayer->SetHidden(true);
	InPlayer->SetActorEnableCollision(false);
	InPlayer->GetCharacterMovement()->SetActive(false);
}

/**
 * @brief	デバッグ表示止めるべきか
 */
bool	UCSDebug_Utility::IsNeedStopDebugDraw(const UWorld* InWorld)
{
#if WITH_EDITOR
	if (InWorld
		&& InWorld->WorldType == EWorldType::Editor)
	{//自分がEditorのときにGame動いてたら表示しない
		const TIndirectArray<FWorldContext>& WorldContextList = GUnrealEd->GetWorldContexts();
		for (auto& WorldContext : WorldContextList)
		{
			if (WorldContext.WorldType == EWorldType::PIE)
			{
				return true;
			}
		}
	}
#endif
	return false;
}

/* ------------------------------------------------------------
   !Actorが所属するレベル名(サブレベル配置だったらサブレベル名)
------------------------------------------------------------ */
FString UCSDebug_Utility::GetActorLevelName(const AActor* InActor)
{
	if (InActor == nullptr)
	{
		return FString();
	}
	if (InActor->GetWorld()->IsEditorWorld())
	{
		return GetActorLevelNameInEditor(InActor);
	}
	return GetActorLevelNameInGame(InActor);
}
FString UCSDebug_Utility::GetActorLevelNameInGame(const AActor* InActor)
{
	if (InActor == nullptr)
	{
		return FString();
	}
	const ULevel* ActorLevel = InActor->GetLevel();
	for (ULevelStreaming* StreamingLevel : InActor->GetWorld()->GetStreamingLevels())
	{
		if (StreamingLevel
			&& StreamingLevel->GetLoadedLevel() == ActorLevel)
		{
			const FString PathString = StreamingLevel->PackageNameToLoad.ToString();
			TArray<FString> StringArray;
			PathString.ParseIntoArray(StringArray, TEXT("/"));
			if (StringArray.Num() > 0)
			{
				return StringArray[StringArray.Num() - 1];
			}
			return PathString;
		}
	}
	return FString();
}

FString UCSDebug_Utility::GetActorLevelNameInEditor(const AActor* InActor)
{
	if (InActor == nullptr)
	{
		return FString();
	}
	const ULevel* Level = InActor->GetLevel();
	if (Level == nullptr)
	{
		return FString();
	}
	const FString LevelPathName = Level->GetPathName();
	TArray<FString> PathList;
	LevelPathName.ParseIntoArray(PathList, TEXT("/"));
	FString LastPath = LevelPathName;
	if (PathList.Num() > 0)
	{
		LastPath = PathList[PathList.Num() - 1];
	}
	FString LevelName;
	FString PersistentLevel;
	LastPath.Split(FString(TEXT(".")), &LevelName, &PersistentLevel);
	return LevelName;
}