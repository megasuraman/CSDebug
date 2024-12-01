// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_EUW_CheckPlacedActor.h
 * @brief �z�uActor�ɑ΂���`�F�b�N
 * @author SensyuGames
 * @date 2024/11/30
 */


#include "CSDebug_EUW_CheckPlacedActor.h"
#include "CSDebugUtility.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Classes/GameFramework/Character.h"
#include "Engine/Classes/GameFramework/WorldSettings.h"

/**
 * @brief	�z�uActor�̃N���X�������x���ʂɊi�[
 */
void UCSDebug_EUW_CheckPlacedActor::SetupPlacedActorClassMap()
{
	mPlacedActorClassMap.Empty();

	const TArray<ULevel*>& Levels = GetWorld()->GetLevels();
	for (ULevel* Level : Levels)
	{
		const FString LevelName = GetLevelName(Level);
		FCSDebug_PlacedActorClassList& PlacedActorClassList = mPlacedActorClassMap.FindOrAdd(LevelName);
		for (const AActor* Actor : Level->Actors)
		{
			if (Actor == nullptr)
			{
				continue;
			}
			if (PlacedActorClassList.mClassList.Find(Actor->GetClass()) == INDEX_NONE)
			{
				PlacedActorClassList.mClassList.Add(Actor->GetClass());
			}
		}
	}
}

/**
 * @brief	�G���[�`�F�b�N
 */
bool UCSDebug_EUW_CheckPlacedActor::CheckError()
{
	bool bError = false;
	bError |= CheckError_PossibleToPlaceActor();
	return bError;
}

/**
 * @brief	�w��̃��x���ɔz�u�z�肶��Ȃ�Actor�����݂��Ȃ����`�F�b�N
 */
bool UCSDebug_EUW_CheckPlacedActor::CheckError_PossibleToPlaceActor()
{
	bool bError = false;
	const TArray<ULevel*>& Levels = GetWorld()->GetLevels();
	for (const auto& MapElement : mPossibleToPlaceActorClassMap)
	{
		const FString TargetLevelNameOrigin = MapElement.Key;
		const FCSDebug_PlacedActorClassList& PossibleToPlaceClassList = MapElement.Value;

		FString TempString;
		FString TargetLevelName;
		if (!TargetLevelNameOrigin.Split(FString(TEXT("*")), &TempString, &TargetLevelName))
		{
			TargetLevelName = TargetLevelNameOrigin;
		}

		for (ULevel* Level : Levels)
		{
			const FString LevelName = GetLevelName(Level);
			if (!LevelName.Contains(TargetLevelName))
			{
				continue;
			}

			for (const AActor* Actor : Level->Actors)
			{
				if (Actor == nullptr
					|| Actor->IsA(AWorldSettings::StaticClass())
					|| Actor->GetClass() == ABrush::StaticClass()
					)
				{
					continue;
				}

				bool bPossibleToPlace = false;
				for (const UClass* PossibleToPlaceAcotrClass : PossibleToPlaceClassList.mClassList)
				{
					if (Actor->IsA(PossibleToPlaceAcotrClass))
					{
						bPossibleToPlace = true;
						break;
					}
				}
				if (!bPossibleToPlace)
				{
					UE_LOG(LogTemp, Error, TEXT("Impossible to place Actor : [%s]<%s> %s"),
						*LevelName,
						*Actor->GetClass()->GetName(),
						*Actor->GetActorLabel()
					);
					bError = true;
				}
			}
		}
	}
	return bError;
}

/**
 * @brief	���x�����擾
 */
FString UCSDebug_EUW_CheckPlacedActor::GetLevelName(const ULevel* InLevel) const
{
	const FString LevelPathName = InLevel->GetPathName();
	TArray<FString> PathList;
	LevelPathName.ParseIntoArray(PathList, TEXT("/"));
	FString LastPath = LevelPathName;
	if (PathList.Num() > 0)
	{
		LastPath = PathList[PathList.Num() - 1];
	}
	FString LevelName;
	FString PersistantLevel;
	LastPath.Split(FString(TEXT(".")), &LevelName, &PersistantLevel);
	return LevelName;
}
