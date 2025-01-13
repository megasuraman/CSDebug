// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_Utility.h
 * @brief ï÷óòä÷êîìôÇÃÇ‹Ç∆Çﬂ(ï™óﬁÇµÇ…Ç≠Ç©Ç¡ÇΩìz)
 * @author SensyuGames
 * @date 2023/02/24
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebug_Utility.generated.h"

class APawn;
struct FEnvQueryInstance;

/**
 * 
 */
UCLASS()
class CSDEBUG_API UCSDebug_Utility : public UObject
{
	GENERATED_BODY()
	
public:
	static FString	GetUEnumString(const TCHAR* InEnumTypeName, int32 InEnumValue);
	static FEnvQueryInstance* FindLastEnvQueryInstance(float& OutLastTimeStamp, const APawn* InOwner);
	static void MakeGhostPlayer(ACharacter* InPlayer, AActor* InTarget);
	static bool	IsNeedStopDebugDraw(const UWorld* InWorld);
	static FString GetActorLevelName(const AActor* InActor);
	static FString GetActorLevelNameInGame(const AActor* InActor);
	static FString GetActorLevelNameInEditor(const AActor* InActor);
};
