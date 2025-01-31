// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_SaveData.h
 * @brief Debug用の簡易セーブ情報(UE5.4からGConfig->Set系で.iniの書き込みがうまく行かなくなったので代わり)
 * @author SensyuGames
 * @date 2025/01/27
 */
#pragma once

#include "CoreMinimal.h"
#include "Serialization/JsonSerializerMacros.h"

enum class ECSDebug_SaveDataValueType : uint8
{
	Invalid,
	Bool,
	Int,
	Float,
	String,
};
struct FCSDebug_SaveDataValue : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("mTag", mTag);
	JSON_SERIALIZE("mValueString", mValueString);
	JSON_SERIALIZE("mValueType", mValueType);
	END_JSON_SERIALIZER

	FString mTag;
	FString mValueString;
	int32 mValueType;

	ECSDebug_SaveDataValueType GetValueType() const{return static_cast<ECSDebug_SaveDataValueType>(mValueType); }
};

struct FCSDebug_SaveData : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE_ARRAY_SERIALIZABLE("mValueList", mValueList, FCSDebug_SaveDataValue);
	END_JSON_SERIALIZER
public:
	CSDEBUG_API void Save();
	CSDEBUG_API void Load();
	CSDEBUG_API void SetBool(const FString& InTag, const bool bInValue);
	CSDEBUG_API void SetInt(const FString& InTag, const int32 InValue);
	CSDEBUG_API void SetFloat(const FString& InTag, const float InValue);
	CSDEBUG_API void SetString(const FString& InTag, const FString& InValue);
	CSDEBUG_API bool GetBool(const FString& InTag) const;
	CSDEBUG_API int32 GetInt(const FString& InTag) const;
	CSDEBUG_API float GetFloat(const FString& InTag) const;
	CSDEBUG_API FString GetString(const FString& InTag) const;

private:
	TArray<FCSDebug_SaveDataValue> mValueList;
	TMap<FString, FCSDebug_SaveDataValue> mValueMap;
	bool mbLoaded = false;
};