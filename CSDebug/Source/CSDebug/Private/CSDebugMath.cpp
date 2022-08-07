// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMath.cpp
 * @brief ŒvŽZŠÖ”“™‚Ì‚Ü‚Æ‚ß
 * @author SensyuGames
 * @date 2022/3/19
 */


#include "CSDebugMath.h"

#include "EnvironmentQuery/EnvQueryManager.h"

/**
 * @brief	ÅI‚ÌEQSî•ñŽæ“¾
 */
FEnvQueryInstance* UCSDebugMath::FindLastEnvQueryInstance(float& OutLastTimeStamp, const APawn* InOwner)
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