// Copyright 2020 SensyuGames.
/**
 * @file CSDebugUtility.cpp
 * @brief �֗��֐����̂܂Ƃ�(���ނ��ɂ��������z)
 * @author SensyuGames
 * @date 2023/02/24
 */
#include "CSDebugUtility.h"

#include "EnvironmentQuery/EnvQueryManager.h"
#include "DrawDebugHelpers.h"

#if WITH_EDITOR
#include "Editor.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#endif

/**
 * @brief	Enum�̗v�f���擾
 */
FString UCSDebugUtility::GetUEnumString(const TCHAR* InEnumTypeName, int32 InEnumValue)
{
	if (const UEnum* EnumPointer = FindObject< UEnum >(ANY_PACKAGE, InEnumTypeName, true))
	{
		return EnumPointer->GetNameStringByValue((int64)InEnumValue);
	}
	return FString::Printf(TEXT("%d"), InEnumValue);
}

/**
 * @brief	�ŏI��EQS���擾
 */
FEnvQueryInstance* UCSDebugUtility::FindLastEnvQueryInstance(float& OutLastTimeStamp, const APawn* InOwner)
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
 * @brief	�f�o�b�O�\���~�߂�ׂ���
 */
bool	UCSDebugUtility::IsNeedStopDebugDraw(const UWorld* InWorld)
{
#if WITH_EDITOR
	if (InWorld->IsEditorWorld())
	{//������EditorWorld�̂Ƃ���Game�����Ă���\�����Ȃ�
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