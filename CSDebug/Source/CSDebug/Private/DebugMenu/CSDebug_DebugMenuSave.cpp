// Copyright 2022 SensyuGames.
#include "DebugMenu/CSDebug_DebugMenuSave.h"

void FCSDebug_DebugMenuSaveData::Clear()
{
	mSaveNodeList.Empty();
	mValueMap.Empty();
}

void FCSDebug_DebugMenuSaveData::Save()
{
	mSaveNodeList.Empty();
	for (const auto& MapElement : mValueMap)
	{
		FCSDebug_DebugMenuSaveDataNode Node;
		Node.mPath = MapElement.Key;
		Node.mValueString = MapElement.Value;
		mSaveNodeList.Add(Node);
	}

	const FString FilePath = GetSaveFilePath();
	FFileHelper::SaveStringToFile(ToJson(), *FilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}

void FCSDebug_DebugMenuSaveData::Load()
{
	if(mbLoaded)
	{
		return;
	}

	Clear();

	const FString FilePath = GetSaveFilePath();	
	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *FilePath);
	FromJson(JsonString);

	for (const FCSDebug_DebugMenuSaveDataNode& Node : mSaveNodeList)
	{
		WriteValue(Node.mPath, Node.mValueString);
	}
	mbLoaded = true;
}

void FCSDebug_DebugMenuSaveData::WriteValue(const FString& InPath, const FString& InValue)
{
	FString& ValueString = mValueMap.FindOrAdd(InPath);
	ValueString = InValue;
}

FString FCSDebug_DebugMenuSaveData::GetValueString(const FString& InPath) const
{
	if (const FString* ValueString = mValueMap.Find(InPath))
	{
		return *ValueString;
	}
	return FString();
}

FString FCSDebug_DebugMenuSaveData::GetSaveFilePath()
{
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FilePath += FString(TEXT("CSDebug/DebugMenu/SaveData.json"));
	return FilePath;
}
