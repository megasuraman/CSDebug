// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMenuNodeData.h
 * @brief CSDebugMenu��Node���
 * @author SensyuGames
 * @date 2020/08/01
 */

#pragma once

#include "CoreMinimal.h"
#include "Containers/UnrealString.h"
#include "Serialization/JsonSerializerMacros.h"
#include "CSDebugMenuNodeData.generated.h"

struct FCSDebugMenuNodeGetter;
DECLARE_DELEGATE_OneParam(FCSDebugMenuNodeDelegate, const FCSDebugMenuNodeGetter&);

UENUM(BlueprintType)
enum class ECSDebugMenuNodeValueKind : uint8
{
	Invalid,
	Bool,
	Int,
	Float,
	Selector,
	Menu,
	Button,
};

// FCSDebugMenuNodeValue�������X�g�^�f�[�^
struct FCSDebugMenuNodeValueKindSelector : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
		JSON_SERIALIZE_ARRAY("mList", mList);
		JSON_SERIALIZE("mIndex", mIndex);
	END_JSON_SERIALIZER

	TArray<FString>	mList;
	int32	mIndex = 0;
};

// �m�[�h���
struct FCSDebugMenuNodeValue : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
		JSON_SERIALIZE_ARRAY_SERIALIZABLE("mList", mList, FCSDebugMenuNodeValue);
		JSON_SERIALIZE("mDispString", mDispString);
		JSON_SERIALIZE("mValueString", mValueString);
		JSON_SERIALIZE("mKind", mKind);
		JSON_SERIALIZE("mbFromEUW", mbFromEUW);
	END_JSON_SERIALIZER

	TArray<FCSDebugMenuNodeValue>	mList;
	FString mDispString;
	FString mValueString;
	int32	mKind = 0;
	bool	mbFromEUW = false;

	//*** ���s���p
	int32	mSelectIndex = 0;
	FCSDebugMenuNodeDelegate	mDelegate;
	bool	mbAddRuntime = false;
	//******//

	void	SetRuntimeNode(const FCSDebugMenuNodeValue& InNodeValue);
	const FString& GetDispString() const { return mDispString; }
	FString	GetValueString() const;
	ECSDebugMenuNodeValueKind GetKind() const { return (ECSDebugMenuNodeValueKind)mKind; }
	bool	GetBool() const;
	int32	GetInt() const;
	float	GetFloat() const;
	bool	GetSelectorList(FCSDebugMenuNodeValueKindSelector& OutList) const;
	void	SetDispString(const FString& InString) { mDispString = InString; }
	void	Set(const bool bInBool);
	void	Set(const int32 InInt);
	void	Set(const float InFloat);
	void	Set(const int32 InListIndex, const TArray<FString>& InList);
	void	SetSelectorListIndex(const int32 InListIndex);
	void	Set(const FCSDebugMenuNodeDelegate& InDelegate);
	void	AddMenuList(const FCSDebugMenuNodeValue& InNodeValue);
	FCSDebugMenuNodeValue* GetCurrentLastNode();
	FCSDebugMenuNodeValue* FindCurrentNode(const FString& InDispString);
	FCSDebugMenuNodeValue* FindAllNode(const FString& InPath);
	const FCSDebugMenuNodeValue* FindAllNode(const FString& InPath) const;
	void	CheckDeleteNode();
	void	CallDelegate();
};