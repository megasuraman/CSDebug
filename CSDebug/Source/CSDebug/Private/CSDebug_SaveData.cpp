// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_SaveData.cpp
 * @brief Debug用の簡易セーブ情報(UE5.4からGConfig->Set系で.iniの書き込みがうまく行かなくなったので代わり)
 * @author SensyuGames
 * @date 2025/01/27
 */
#include "CSDebug_SaveData.h"

void FCSDebug_SaveData::Save()
{
	mValueList.Empty();
	for (const auto& MapElement : mValueMap)
	{
		mValueList.Add(MapElement.Value);
	}

	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FilePath += FString(TEXT("CSDebug/SaveData.txt"));
	FFileHelper::SaveStringToFile(ToJson(), *FilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}

void FCSDebug_SaveData::Load()
{
	if(mbLoaded)
	{
		return;
	}
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FilePath += FString(TEXT("CSDebug/SaveData.txt"));
	
	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *FilePath);
	FromJson(JsonString);
	mbLoaded = true;

	mValueMap.Empty();
	for(const FCSDebug_SaveDataValue& SaveDataValue : mValueList)
	{
		mValueMap.Add(SaveDataValue.mTag, SaveDataValue);
	}
}

void FCSDebug_SaveData::SetBool(const FString& InTag, bool bInValue)
{
	if (FCSDebug_SaveDataValue* SaveDataValue = mValueMap.Find(InTag))
	{
		ensure(SaveDataValue->GetValueType() == ECSDebug_SaveDataValueType::Bool);
		if (bInValue)
		{
			SaveDataValue->mValueString = FString(TEXT("True"));
		}
		else
		{
			SaveDataValue->mValueString = FString(TEXT("False"));
		}
		Save();
		return;
	}

	//新規追加
	FCSDebug_SaveDataValue Value;
	Value.mTag = InTag;
	if(bInValue)
	{
		Value.mValueString = FString(TEXT("True"));
	}
	else
	{
		Value.mValueString = FString(TEXT("False"));
	}
	Value.mValueType = static_cast<int32>(ECSDebug_SaveDataValueType::Bool);
	mValueMap.Add(InTag, Value);
	Save();
}

bool FCSDebug_SaveData::GetBool(const FString& InTag) const
{
	if (const FCSDebug_SaveDataValue* SaveDataValue = mValueMap.Find(InTag))
	{
		ensure(SaveDataValue->GetValueType() == ECSDebug_SaveDataValueType::Bool);
		return SaveDataValue->mValueString.ToBool();
	}
	return false;
}
