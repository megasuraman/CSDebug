// Copyright 2022 SensyuGames.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DebugMenu/CSDebug_DebugMenuNodeBase.h"
#include "DebugMenu/CSDebug_DebugMenuSave.h"
#include "CSDebug_DebugMenuManager.generated.h"

class CSDebug_DebugMenuNodeBase;

UCLASS()
class CSDEBUG_API UCSDebug_DebugMenuManager : public UObject
{
	GENERATED_BODY()

	struct FFolder;
	
public:
	static UCSDebug_DebugMenuManager* sGet(const UObject* InObject);

	UCSDebug_DebugMenuManager();
	virtual void BeginDestroy() override;

	void Init();
	void DebugTick(const float InDeltaTime);
	void DebugDraw(UCanvas* InCanvas);
	CSDebug_DebugMenuNodeBase* AddNode(const FString& InFolderPath, const FCSDebug_DebugMenuNodeData& InNodeData);
	CSDebug_DebugMenuNodeBase* AddNode_Bool(const FString& InFolderPath, const FString& InDisplayName, const bool InInitValue);
	CSDebug_DebugMenuNodeBase* AddNode_Button(const FString& InFolderPath, const FString& InDisplayName, const FCSDebug_DebugMenuNodeActionDelegate& InDelegate);
	bool GetNodeValue_Bool(const FString& InPath) const;
	void SetNodeActionDelegate(const FString& InPath, const FCSDebug_DebugMenuNodeActionDelegate& InDelegate);
	void SetMainFolder(const FString& InPath);
	void BackMainFolder();
	void SetActive(const bool bInActive);
	bool IsActive() const {return mbActive;}

protected:
	void SetupDefaultMenu();
	void ClearNode();
	APlayerController* FindPlayerController() const;
	void ChangeSelectNode(const bool bInDown);
	void DrawMainFolderPath(UCanvas* InCanvas, const FVector2D& InPos) const;
	FFolder& FindOrAddFolder(const FString& InPath);
	CSDebug_DebugMenuNodeBase* FindOrAddDebugMenuNodeFolder(const FString& InPath);
	CSDebug_DebugMenuNodeBase* FindDebugMenuNode(const FString& InPath);
	void AssignNodeToFolder(CSDebug_DebugMenuNodeBase* InNode);
	FString CheckPathString(const FString& InPath) const;
	void Save(const FCSDebug_DebugMenuNodeActionParameter& InParameter);
	void Load(const FCSDebug_DebugMenuNodeActionParameter& InParameter);

private:
	struct FFolder
	{
		TArray<CSDebug_DebugMenuNodeBase*> mNodeList;
		FString mPath;
	};
	TMap<FString, CSDebug_DebugMenuNodeBase*> mNodeMap;
	TMap<FString, FFolder> mFolderMap;
	FCSDebug_DebugMenuSaveData mSaveData;
	FString mMainFolderPath;
	FString mRootPath = FString(TEXT("~"));
	CSDebug_DebugMenuNodeBase* mSelectNode = nullptr;
	bool mbActive = false;
	bool mbDoneAutoLoad = false;
};
