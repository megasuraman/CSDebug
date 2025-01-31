// Copyright 2022 SensyuGames.
#include "DebugMenu/CSDebug_DebugMenuManager.h"

#include "CSDebug_Subsystem.h"
#include "DebugMenu/CSDebug_DebugMenuManager.h"
#include "DebugMenu/CSDebug_DebugMenuNodeBool.h"
#include "DebugMenu/CSDebug_DebugMenuNodeInt.h"
#include "DebugMenu/CSDebug_DebugMenuNodeFloat.h"
#include "DebugMenu/CSDebug_DebugMenuNodeFolder.h"
#include "DebugMenu/CSDebug_DebugMenuNodeList.h"
#include "DebugMenu/CSDebug_DebugMenuNodeButton.h"
#include "DebugMenu/CSDebug_DebugMenuTableRow.h"

UCSDebug_DebugMenuManager* UCSDebug_DebugMenuManager::sGet(const UObject* InObject)
{
	UGameInstance* GameInstance = InObject->GetWorld()->GetGameInstance();
	UCSDebug_Subsystem* CSDebugSubsystem = GameInstance->GetSubsystem<UCSDebug_Subsystem>();
	return CSDebugSubsystem->GetDebugMenuManager();
}

UCSDebug_DebugMenuManager::UCSDebug_DebugMenuManager()
{
}

void UCSDebug_DebugMenuManager::BeginDestroy()
{
	Super::BeginDestroy();

	ClearNode();
}

void UCSDebug_DebugMenuManager::Init()
{
	ClearNode();

	const UCSDebug_Config* CSDebugConfig = GetDefault<UCSDebug_Config>();
	const UDataTable* DataTable = CSDebugConfig->mDebugMenuDataTable.LoadSynchronous();
	if (DataTable == nullptr)
	{
		return;
	}

	FindOrAddFolder(mRootPath);

	TArray<FName> RowNameList = DataTable->GetRowNames();
	for (const FName& RowName : RowNameList)
	{
		const FCSDebug_DebugMenuTableRow* DebugMenuTableRow = DataTable->FindRow<FCSDebug_DebugMenuTableRow>(RowName, FString());
		if (DebugMenuTableRow == nullptr)
		{
			continue;
		}

		FFolder NodeFolder;
		for(const FCSDebug_DebugMenuNodeData& NodeData : DebugMenuTableRow->mNodeList)
		{
			AddNode(RowName.ToString(), NodeData);
		}
	}

	SetupDefaultMenu();

	SetMainFolder(mRootPath);


	if (UCSDebug_Subsystem::sGetSaveData().GetBool(FString(TEXT("DebugMenu_AutoLoad"))))
	{
		Load(FCSDebug_DebugMenuNodeActionParameter());
		mbDoneAutoLoad = true;
	}
}

void UCSDebug_DebugMenuManager::DebugTick(const float InDeltaTime)
{
	if (!mbActive)
	{
		return;
	}
	if (mSelectNode == nullptr)
	{
		return;
	}
	APlayerController* PlayerController = FindPlayerController();
	if (PlayerController == nullptr)
	{
		return;
	}

	const UPlayerInput* PlayerInput = PlayerController->PlayerInput;
	if (PlayerInput == nullptr)
	{
		return;
	}

	const UCSDebug_Config* CSDebugConfig = GetDefault<UCSDebug_Config>();
	if (CSDebugConfig->mDebugCommand_ReadyKey.IsPressed(*PlayerInput))
	{
		return;
	}

	const bool bPressedSelectKey = CSDebugConfig->mDebugMenu_SelectKey.IsPressed(*PlayerInput);
	if (CSDebugConfig->mDebugMenu_SelectKey.IsJustPressed(*PlayerInput))
	{
		mSelectNode->OnBeginAction();
	}
	else if (CSDebugConfig->mDebugMenu_SelectKey.IsJustReleased(*PlayerInput))
	{
		FCSDebug_DebugMenuNodeActionParameter ActionParameter;
		ActionParameter.mPlayerController = PlayerController;
		mSelectNode->OnEndAction(ActionParameter);
	}
	else if (CSDebugConfig->mDebugMenu_CancelKey.IsJustPressed(*PlayerInput))
	{
	}
	else if (CSDebugConfig->mDebugMenu_UpKey.IsJustPressed(*PlayerInput))
	{
		mSelectNode->OnJustPressedUpKey();
		if (!bPressedSelectKey)
		{
			ChangeSelectNode(false);
		}
	}
	else if (CSDebugConfig->mDebugMenu_DownKey.IsJustPressed(*PlayerInput))
	{
		mSelectNode->OnJustPressedDownKey();
		if (!bPressedSelectKey)
		{
			ChangeSelectNode(true);
		}
	}
	else if (CSDebugConfig->mDebugMenu_LeftKey.IsJustPressed(*PlayerInput))
	{
		mSelectNode->OnJustPressedLeftKey();
		if (!bPressedSelectKey)
		{
			BackMainFolder();
		}
	}
	else if (CSDebugConfig->mDebugMenu_RightKey.IsJustPressed(*PlayerInput))
	{
		mSelectNode->OnJustPressedRightKey();
	}
}

void UCSDebug_DebugMenuManager::DebugDraw(UCanvas* InCanvas)
{
	if (!mbActive)
	{
		return;
	}

	FVector2D DrawPos(50.f, 30.f);

	DrawMainFolderPath(InCanvas, DrawPos);
	DrawPos.Y += 20.f;

	FVector2D SelectNodeDrawPos = DrawPos;
	if (const FFolder* MainFolder = mFolderMap.Find(mMainFolderPath))
	{
		for(const CSDebug_DebugMenuNodeBase* Node : MainFolder->mNodeList)
		{
			if (mSelectNode == Node)
			{
				SelectNodeDrawPos = DrawPos;//選択されてるのは最後に描画したいので
			}
			else
			{
				Node->Draw(InCanvas, DrawPos, false);
			}
			DrawPos.Y += 20.f;
		}
		if (mSelectNode)
		{
			mSelectNode->Draw(InCanvas, SelectNodeDrawPos, true);
		}
	}
}

CSDebug_DebugMenuNodeBase* UCSDebug_DebugMenuManager::AddNode(const FString& InFolderPath, const FCSDebug_DebugMenuNodeData& InNodeData)
{
	const FString PathString = CheckPathString(InFolderPath);
	const FString NodePath = FString::Printf(TEXT("%s/%s"), *PathString, *InNodeData.mDisplayName);
	if (CSDebug_DebugMenuNodeBase** NodeBase = mNodeMap.Find(NodePath))
	{
		return *NodeBase;
	}

	CSDebug_DebugMenuNodeBase* NewNode = nullptr;
	switch (InNodeData.mKind)
	{
	case ECSDebug_DebugMenuValueKind::Bool:
		NewNode = new CSDebug_DebugMenuNodeBool();
		break;
	case ECSDebug_DebugMenuValueKind::Int:
		NewNode = new CSDebug_DebugMenuNodeInt();
		break;
	case ECSDebug_DebugMenuValueKind::Float:
		NewNode = new CSDebug_DebugMenuNodeFloat();
		break;
	case ECSDebug_DebugMenuValueKind::List:
	case ECSDebug_DebugMenuValueKind::Enum:
		NewNode = new CSDebug_DebugMenuNodeList();
		break;
	case ECSDebug_DebugMenuValueKind::Button:
		NewNode = new CSDebug_DebugMenuNodeButton();
		break;
	default:
		NewNode = new CSDebug_DebugMenuNodeBase();
		break;
	}

	NewNode->Init(PathString, InNodeData, this);
	mNodeMap.Add(NewNode->GetPath(), NewNode);
	AssignNodeToFolder(NewNode);
	if (mbDoneAutoLoad)
	{
		const FString SaveValue = mSaveData.GetValueString(NewNode->GetPath());
		if(!SaveValue.IsEmpty())
		{
			NewNode->Load(SaveValue, FCSDebug_DebugMenuNodeActionParameter());
		}
	}
	return NewNode;
}

CSDebug_DebugMenuNodeBase* UCSDebug_DebugMenuManager::AddNode_Bool(const FString& InFolderPath, const FString& InDisplayName, const bool InInitValue)
{
	FCSDebug_DebugMenuNodeData NodeData;
	NodeData.mDisplayName = InDisplayName;
	NodeData.mKind = ECSDebug_DebugMenuValueKind::Bool;
	NodeData.mInitValue = InInitValue ? FString(TEXT("true")) : FString(TEXT("false"));
	return AddNode(InFolderPath, NodeData);
}

CSDebug_DebugMenuNodeBase* UCSDebug_DebugMenuManager::AddNode_Button(const FString& InFolderPath, const FString& InDisplayName, const FCSDebug_DebugMenuNodeActionDelegate& InDelegate)
{
	FCSDebug_DebugMenuNodeData NodeData;
	NodeData.mDisplayName = InDisplayName;
	NodeData.mKind = ECSDebug_DebugMenuValueKind::Button;
	if (CSDebug_DebugMenuNodeBase* DebugMenuNode = AddNode(InFolderPath, NodeData))
	{
		DebugMenuNode->SetNodeAction(InDelegate);
		return DebugMenuNode;
	}
	return nullptr;
}

bool UCSDebug_DebugMenuManager::GetNodeValue_Bool(const FString& InPath) const
{
	const FString PathString = CheckPathString(InPath);
	if (const CSDebug_DebugMenuNodeBase* const* NodePtrPtr = mNodeMap.Find(PathString))
	{
		if (const CSDebug_DebugMenuNodeBase* NodePtr = *NodePtrPtr)
		{
			return NodePtr->GetBool();
		}
	}
	return false;
}

void UCSDebug_DebugMenuManager::SetNodeActionDelegate(const FString& InPath, const FCSDebug_DebugMenuNodeActionDelegate& InDelegate)
{
	if (CSDebug_DebugMenuNodeBase** NodePtrPtr = mNodeMap.Find(InPath))
	{
		if (CSDebug_DebugMenuNodeBase* NodePtr = *NodePtrPtr)
		{
			NodePtr->SetNodeAction(InDelegate);
		}
	}
}

void UCSDebug_DebugMenuManager::SetMainFolder(const FString& InPath)
{
	mMainFolderPath = InPath;

	FFolder& RootFolder = FindOrAddFolder(mMainFolderPath);
	if (RootFolder.mNodeList.Num() > 0)
	{
		mSelectNode = RootFolder.mNodeList[0];
	}
}

void UCSDebug_DebugMenuManager::BackMainFolder()
{
	TArray<FString> FolderList;
	mMainFolderPath.ParseIntoArray(FolderList, TEXT("/"));
	const int32 FolderNum = FolderList.Num();
	if (FolderNum <= 1)
	{
		return;
	}

	FString BackFolderPath;
	for (int32 i = 0; i < FolderNum - 1; ++i)
	{
		if (i != 0)
		{
			BackFolderPath += FString::Printf(TEXT("/"));
		}
		BackFolderPath += FolderList[i];
	}
	SetMainFolder(BackFolderPath);
}

void UCSDebug_DebugMenuManager::SetActive(const bool bInActive)
{
	mbActive = bInActive;
}

void UCSDebug_DebugMenuManager::SetupDefaultMenu()
{
	const FString BaseDebugMenuPath(TEXT("CSDebug/DebugMenu"));
	{
		const auto& Delegate = FCSDebug_DebugMenuNodeActionDelegate::CreateUObject(this, &UCSDebug_DebugMenuManager::Save);
		AddNode_Button(BaseDebugMenuPath, FString(TEXT("Save")), Delegate);
	}
	{
		const auto& Delegate = FCSDebug_DebugMenuNodeActionDelegate::CreateUObject(this, &UCSDebug_DebugMenuManager::Load);
		AddNode_Button(BaseDebugMenuPath, FString(TEXT("Load")), Delegate);
	}
}

void UCSDebug_DebugMenuManager::ClearNode()
{
	for (auto& MapElement : mNodeMap)
	{
		CSDebug_DebugMenuNodeBase* Node = MapElement.Value;
		delete Node;
	}
	mNodeMap.Empty();
}

APlayerController* UCSDebug_DebugMenuManager::FindPlayerController() const
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PlayerController = Iterator->Get())
		{
			if (PlayerController->Player)
			{
				return PlayerController;
			}
		}
	}
	return nullptr;
}

void UCSDebug_DebugMenuManager::ChangeSelectNode(const bool bInDown)
{
	const FFolder* NodeFolder = mFolderMap.Find(mMainFolderPath);
	if (NodeFolder == nullptr)
	{
		return;
	}
	const TArray<CSDebug_DebugMenuNodeBase*>& NodeList = NodeFolder->mNodeList;
	const int32 NodeListSize = NodeList.Num();
	if (NodeListSize <= 0)
	{
		return;
	}
	int32 SelectIndex = NodeList.Find(mSelectNode);
	if (SelectIndex == INDEX_NONE)
	{
		return;
	}
	if (bInDown)
	{
		SelectIndex = (SelectIndex + 1) % NodeListSize;
	}
	else
	{
		if (SelectIndex == 0)
		{
			SelectIndex = NodeListSize - 1;
		}
		else
		{
			SelectIndex = SelectIndex - 1;
		}
	}
	mSelectNode = NodeList[SelectIndex];
}

void UCSDebug_DebugMenuManager::DrawMainFolderPath(UCanvas* InCanvas, const FVector2D& InPos) const
{
	const FVector2D WindowExtent(200.f, 20.f);
	const float ValueLineOffsetX = 150.f;
	const FVector2D StringOffset(2.f, 2.f);
	const FLinearColor WindowBackColor(0.01f, 0.01f, 0.01f, 0.5f);
	const FLinearColor WindowFrameColor(0.1f, 0.9f, 0.1f, 1.f);
	const FLinearColor FontColor(0.1f, 0.9f, 0.1f, 1.f);
	// 下敷き
	{
		FCanvasTileItem Item(InPos, WindowExtent, WindowBackColor);
		Item.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;
		InCanvas->DrawItem(Item);
	}
	// 枠
	{
		FCanvasBoxItem Item(InPos, WindowExtent);
		Item.SetColor(WindowFrameColor);
		Item.LineThickness = 1.f;
		InCanvas->DrawItem(Item);
	}
	// パス表示
	{
		const FVector2D StringPos = InPos + StringOffset;
		FCanvasTextItem Item(StringPos, FText::FromString(mMainFolderPath), GEngine->GetSmallFont(), FontColor);
		Item.Scale = FVector2D(1.f);
		InCanvas->DrawItem(Item);
	}
}

UCSDebug_DebugMenuManager::FFolder& UCSDebug_DebugMenuManager::FindOrAddFolder(const FString& InPath)
{
	if (FFolder* NodeFolder = mFolderMap.Find(InPath))
	{
		return *NodeFolder;
	}

	FFolder NodeFolder;
	NodeFolder.mPath = InPath;

	return mFolderMap.Add(InPath, NodeFolder);
}

CSDebug_DebugMenuNodeBase* UCSDebug_DebugMenuManager::FindOrAddDebugMenuNodeFolder(const FString& InPath)
{
	TArray<FString> PathList;
	InPath.ParseIntoArray(PathList, TEXT("/"));
	if (PathList.Num() <= 1)
	{
		return nullptr;
	}

	if (CSDebug_DebugMenuNodeBase** NodePtrPtr = mNodeMap.Find(InPath))
	{
		CSDebug_DebugMenuNodeBase* NodePtr = *NodePtrPtr;
		check(NodePtr->GetNodeData().mKind == ECSDebug_DebugMenuValueKind::Folder);
		return NodePtr;
	}

	FCSDebug_DebugMenuNodeData NodeData;
	NodeData.mKind = ECSDebug_DebugMenuValueKind::Folder;
	NodeData.mDisplayName = PathList[PathList.Num() - 1];

	CSDebug_DebugMenuNodeFolder* NodeFolder = new CSDebug_DebugMenuNodeFolder();
	NodeFolder->Init(InPath, NodeData, this);
	mNodeMap.Add(InPath, NodeFolder);

	FString ParentFolderPath = PathList[0];
	for (int32 i = 1; i < PathList.Num() - 1; ++i)
	{
		ParentFolderPath += FString(TEXT("/")) + PathList[i];
	}
	FFolder& Folder = FindOrAddFolder(ParentFolderPath);
	Folder.mNodeList.Add(NodeFolder);

	return NodeFolder;
}

CSDebug_DebugMenuNodeBase* UCSDebug_DebugMenuManager::FindDebugMenuNode(const FString& InPath)
{
	if (CSDebug_DebugMenuNodeBase** NodePtr = mNodeMap.Find(InPath))
	{
		if (NodePtr != nullptr)
		{
			return *NodePtr;
		}
	}
	return nullptr;
}

void UCSDebug_DebugMenuManager::AssignNodeToFolder(CSDebug_DebugMenuNodeBase* InNode)
{
	const FString NodePath = InNode->GetPath();
	FString FolderPath;
	TArray<FString> PathList;
	NodePath.ParseIntoArray(PathList, TEXT("/"));
	int32 LastIndex = PathList.Num();
	if (InNode->GetNodeData().mKind != ECSDebug_DebugMenuValueKind::Folder)
	{
		--LastIndex;
	}
	for (int32 i = 0; i < LastIndex; ++i)
	{
		if (i != 0)
		{
			FolderPath += FString(TEXT("/"));
		}
		FolderPath += PathList[i];
		FindOrAddDebugMenuNodeFolder(FolderPath);
	}

	FFolder& LastFolder = FindOrAddFolder(FolderPath);
	LastFolder.mNodeList.Add(InNode);
}

FString UCSDebug_DebugMenuManager::CheckPathString(const FString& InPath) const
{
	FString PathString = InPath;
	if (PathString.Left(1) != mRootPath)
	{
		PathString = mRootPath + FString(TEXT("/")) + PathString;
	}
	return PathString;
}

void UCSDebug_DebugMenuManager::Save(const FCSDebug_DebugMenuNodeActionParameter& InParameter)
{
	for (const auto& MapElement : mNodeMap)
	{
		const FString& Path = MapElement.Key;
		if (const CSDebug_DebugMenuNodeBase* Node = MapElement.Value)
		{
			const FCSDebug_DebugMenuNodeData& NodeData = Node->GetNodeData();
			if (NodeData.mKind == ECSDebug_DebugMenuValueKind::Button
				|| NodeData.mKind == ECSDebug_DebugMenuValueKind::Folder)
			{
				continue;
			}
			const FString ValueString = Node->GetValueString();
			mSaveData.WriteValue(Path, ValueString);
		}
	}
	mSaveData.Save();
}

void UCSDebug_DebugMenuManager::Load(const FCSDebug_DebugMenuNodeActionParameter& InParameter)
{
	mSaveData.Load();
	for (const auto& MapElement : mSaveData.GetValueMap())
	{
		if (CSDebug_DebugMenuNodeBase* Node = FindDebugMenuNode(MapElement.Key))
		{
			Node->Load(MapElement.Value, InParameter);
		}
	}
}
