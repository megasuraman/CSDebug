// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMenuManager.cpp
 * @brief CSDebugMenuのManagerクラス
 * @author SensyuGames
 * @date 2020/08/01
 */


#include "DebugMenu/CSDebugMenuManager.h"
#include "CSDebugConfig.h"
#include "DebugInfoWindow/CSDebugInfoWindowText.h"
#include "DebugMenu/CSDebugMenuNodeData.h"
#include "DebugMenu/CSDebugMenuWidgetBase.h"
#include "CSDebugSubsystem.h"

#include "Engine/Canvas.h"
#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"
#include "Engine/DebugCameraController.h"
#include "Blueprint/UserWidget.h"
#include "Windows/WindowsPlatformMisc.h"

UCSDebugMenuManager::UCSDebugMenuManager()
    : mbActive(false)
{
}

void    UCSDebugMenuManager::BeginDestroy()
{
    Super::BeginDestroy();
}

#if USE_CSDEBUG
/**
 * @brief	Get
 */
UCSDebugMenuManager*	UCSDebugMenuManager::Get(UObject* InOwner)
{
	UGameInstance* GameInstance = InOwner->GetWorld()->GetGameInstance();
	UCSDebugSubsystem* CSDebugSubsystem = GameInstance->GetSubsystem<UCSDebugSubsystem>();
	return CSDebugSubsystem->GetDebugMenuManager();
}
#endif //USE_CSDEBUG

/**
 * @brief	Init
 */
void	UCSDebugMenuManager::Init()
{
	Load();
	SetCurrentMenuPath((TEXT("/")));

	const FString BaseDebugMenuPath(TEXT("CSDebug/DebugMenu/"));
	const auto& Delegate = FCSDebugMenuNodeDelegate::CreateUObject(this, &UCSDebugMenuManager::OnPushSaveButton);
	AddButtonNode(BaseDebugMenuPath + TEXT("Save"), Delegate);

	const FString BaseSystemMenuPath(TEXT("CSDebug/System/"));
	AddNodePropertyBool(BaseSystemMenuPath + TEXT("Disp"), mbDispPlatformInfo);
}

/**
 * @brief	Tick
 */
bool	UCSDebugMenuManager::DebugTick(float InDeltaSecond)
{
	APlayerController* PlayerController = FindPlayerController();
	if (PlayerController == nullptr)
	{
		return true;
	}

	const UPlayerInput* PlayerInput = PlayerController->PlayerInput;
	if (PlayerInput == nullptr)
	{
		return true;
	}

	if (mMenuWidget)
	{
		mMenuWidget->DebugTick(*PlayerInput, InDeltaSecond);
	}

#if 0//テスト
	if (GetValueBool(TEXT("SubMenu/SubSubMenuA/TestBool")))
	{
		int32 i = 0;
		++i;
	}
	int32 IntValue = GetValueInt(TEXT("SubMenu/SubSubMenuA/TestInt"));
	if (IntValue > 0)
	{
		int32 i = 0;
		++i;
	}
	//if (const FCSDebugMenuNodeValue* NodeValue = FindNode(TEXT("SubMenu/SubSubMenuA/TestBool")))
	//{
	//	NodeValue->mDelegate.ExecuteIfBound(*NodeValue);
	//}
#endif
    return true;
}

/**
 * @brief	Draw
 */
void	UCSDebugMenuManager::DebugDraw(UCanvas* InCanvas)
{
	if (mbDispPlatformInfo)
	{
		DebugDrawPlatformInfo(InCanvas);
	}
}

/**
 * @brief	SetActive
 */
void	UCSDebugMenuManager::SetActive(const bool bInActive)
{
	if (mbActive == bInActive)
	{
		return;
	}
	mbActive = bInActive;

	if (mbActive)
	{
		CreateMenuWidget();
	}
	else
	{
		DeleteMenuWidget();
	}
}
/**
 * @brief	指定パスの値変更時のコールバック設定
 */
void	UCSDebugMenuManager::SetChangeValueCallback(const FString& InPath, const FCSDebugMenuNodeDelegate& InDelegate)
{
	if (FCSDebugMenuNodeValue* NodeValue = FindNode(InPath))
	{
		NodeValue->mDelegate = InDelegate;
	}
}
/**
 * @brief	ノード追加
 */
void	UCSDebugMenuManager::AddValueNode(const FString& InPathNode, const bool bInInitValue, const FCSDebugMenuNodeDelegate& InDelegate)
{
	FString Path;
	FString NodeName;
	InPathNode.Split(TEXT("/"), &Path, &NodeName, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
	if (FCSDebugMenuNodeValue* MenuNode = FindOrCreateMenuNode(Path))
	{
		FCSDebugMenuNodeValue NodeValue;
		NodeValue.SetDispString(NodeName);
		NodeValue.mDelegate = InDelegate;//Set()で実行したいので先に設定
		bool bInitValue = bInInitValue;
		if (const FCSDebugMenuNodeValue* LoadNode = mLoadAllMenuInstance.FindAllNode(InPathNode))
		{
			bInitValue = LoadNode->GetBool();
		}
		NodeValue.Set(bInitValue);
		NodeValue.mbAddRuntime = true;
		MenuNode->AddMenuList(NodeValue);
	}
}
void	UCSDebugMenuManager::AddValueNode(const FString& InPathNode, const int32 InInitValue, const FCSDebugMenuNodeDelegate& InDelegate)
{
	FString Path;
	FString NodeName;
	InPathNode.Split(TEXT("/"), &Path, &NodeName, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
	if (FCSDebugMenuNodeValue* MenuNode = FindOrCreateMenuNode(Path))
	{
		FCSDebugMenuNodeValue NodeValue;
		NodeValue.SetDispString(NodeName);
		NodeValue.mDelegate = InDelegate;//Set()で実行したいので先に設定
		int32 InitValue = InInitValue;
		if (const FCSDebugMenuNodeValue* LoadNode = mLoadAllMenuInstance.FindAllNode(InPathNode))
		{
			InitValue = LoadNode->GetInt();
		}
		NodeValue.Set(InitValue);
		NodeValue.mbAddRuntime = true;
		MenuNode->AddMenuList(NodeValue);
	}
}
void	UCSDebugMenuManager::AddValueNode(const FString& InPathNode, const float InInitValue, const FCSDebugMenuNodeDelegate& InDelegate)
{
	FString Path;
	FString NodeName;
	InPathNode.Split(TEXT("/"), &Path, &NodeName, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
	if (FCSDebugMenuNodeValue* MenuNode = FindOrCreateMenuNode(Path))
	{
		FCSDebugMenuNodeValue NodeValue;
		NodeValue.SetDispString(NodeName);
		NodeValue.mDelegate = InDelegate;//Set()で実行したいので先に設定
		float InitValue = InInitValue;
		if (const FCSDebugMenuNodeValue* LoadNode = mLoadAllMenuInstance.FindAllNode(InPathNode))
		{
			InitValue = LoadNode->GetFloat();
		}
		NodeValue.Set(InitValue);
		NodeValue.mbAddRuntime = true;
		MenuNode->AddMenuList(NodeValue);
	}
}
void	UCSDebugMenuManager::AddValueNode(const FString& InPathNode, const TArray<FString>& InList, const int32 InSelectIndex, const FCSDebugMenuNodeDelegate& InDelegate)
{
	FString Path;
	FString NodeName;
	InPathNode.Split(TEXT("/"), &Path, &NodeName, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
	if (FCSDebugMenuNodeValue* MenuNode = FindOrCreateMenuNode(Path))
	{
		FCSDebugMenuNodeValue NodeValue;
		NodeValue.SetDispString(NodeName);
		NodeValue.mDelegate = InDelegate;//Set()で実行したいので先に設定
		int32 InitSelectIndex = InSelectIndex;
		if (const FCSDebugMenuNodeValue* LoadNode = mLoadAllMenuInstance.FindAllNode(InPathNode))
		{
			FCSDebugMenuNodeValueKindSelector SelectorData;
			if (LoadNode->GetSelectorList(SelectorData))
			{
				InitSelectIndex = SelectorData.mIndex;
			}
			InitSelectIndex = LoadNode->GetInt();
		}
		NodeValue.Set(InitSelectIndex, InList);
		NodeValue.mbAddRuntime = true;
		MenuNode->AddMenuList(NodeValue);
	}
}
void	UCSDebugMenuManager::AddButtonNode(const FString& InPathNode, const FCSDebugMenuNodeDelegate& InDelegate)
{
	FString Path;
	FString NodeName;
	InPathNode.Split(TEXT("/"), &Path, &NodeName, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
	if (FCSDebugMenuNodeValue* MenuNode = FindOrCreateMenuNode(Path))
	{
		FCSDebugMenuNodeValue NodeValue;
		NodeValue.SetDispString(NodeName);
		NodeValue.Set(InDelegate);
		NodeValue.mbAddRuntime = true;
		MenuNode->AddMenuList(NodeValue);
	}
}


/**
 * @brief	指定パスのNode取得
 */
FCSDebugMenuNodeValue* UCSDebugMenuManager::FindNode(const FString& InPath)
{
	return mAllMenuInstance.FindAllNode(InPath);
}
const FCSDebugMenuNodeValue* UCSDebugMenuManager::FindNode(const FString& InPath) const
{
	return mAllMenuInstance.FindAllNode(InPath);
}
/**
 * @brief	値取得
 */
bool	UCSDebugMenuManager::GetValueBool(const FString& InPath) const
{
	if (const FCSDebugMenuNodeValue* NodeValue = FindNode(InPath))
	{
		if (NodeValue->GetKind() == ECSDebugMenuNodeValueKind::Bool)
		{
			return NodeValue->GetBool();
		}
	}
	return false;
}
int32	UCSDebugMenuManager::GetValueInt(const FString& InPath) const
{
	if (const FCSDebugMenuNodeValue* NodeValue = FindNode(InPath))
	{
		if (NodeValue->GetKind() == ECSDebugMenuNodeValueKind::Int)
		{
			return NodeValue->GetInt();
		}
	}
	return 0;
}
float	UCSDebugMenuManager::GetValueFloat(const FString& InPath) const
{
	if (const FCSDebugMenuNodeValue* NodeValue = FindNode(InPath))
	{
		if (NodeValue->GetKind() == ECSDebugMenuNodeValueKind::Float)
		{
			return NodeValue->GetFloat();
		}
	}
	return 0.f;
}
int32	UCSDebugMenuManager::GetValueSelectIndex(const FString& InPath) const
{
	if (const FCSDebugMenuNodeValue* NodeValue = FindNode(InPath))
	{
		if (NodeValue->GetKind() == ECSDebugMenuNodeValueKind::Selector)
		{
			FCSDebugMenuNodeValueKindSelector NodeValueSelector;
			if (NodeValue->GetSelectorList(NodeValueSelector))
			{
				return NodeValueSelector.mIndex;
			}
		}
	}
	return INDEX_NONE;
}
FString	UCSDebugMenuManager::GetValueSelectString(const FString& InPath) const
{
	if (const FCSDebugMenuNodeValue* NodeValue = FindNode(InPath))
	{
		if (NodeValue->GetKind() == ECSDebugMenuNodeValueKind::Selector)
		{
			FCSDebugMenuNodeValueKindSelector NodeValueSelector;
			if (NodeValue->GetSelectorList(NodeValueSelector)
				&& NodeValueSelector.mIndex < NodeValueSelector.mList.Num())
			{
				return NodeValueSelector.mList[NodeValueSelector.mIndex];
			}
		}
	}
	return FString();
}

/**
 * @brief	有効なPlayerControllerを探す
 */
APlayerController*	UCSDebugMenuManager::FindPlayerController() const
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

/**
 * @brief	Widget作成
 */
void	UCSDebugMenuManager::CreateMenuWidget()
{
	const UCSDebugConfig* CSDebugConfig = GetDefault<UCSDebugConfig>();
	mMenuWidget = CreateWidget<UCSDebugMenuWidgetBase>(GetWorld(), CSDebugConfig->mDebugMenu_WidgetClass);
	if (mMenuWidget != nullptr)
	{
		mMenuWidget->AddToViewport();
		mMenuWidget->Init(this);
		mMenuWidget->SetRenderTranslation(FVector2D(20.f, 20.f));
	}
}

/**
 * @brief	Widget削除
 */
void	UCSDebugMenuManager::DeleteMenuWidget()
{
	if (mMenuWidget != nullptr)
	{
		mMenuWidget->RemoveFromViewport();
		mMenuWidget = nullptr;

		//Save();
	}
}

/**
 * @brief	Saveファイルのパス
 */
FString	UCSDebugMenuManager::GetSaveFilePath() const
{
	FString SaveFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	SaveFilePath += FString(TEXT("/CSDebug/DebugMenu.json"));
	return SaveFilePath;
}

/**
 * @brief	Save
 */
bool	UCSDebugMenuManager::Save()
{
	const FString SaveFilePath = GetSaveFilePath();
	mAllMenuInstance.CheckDeleteNode();
	return FFileHelper::SaveStringToFile(mAllMenuInstance.ToJson(), *SaveFilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}

/**
 * @brief	Load
 */
bool	UCSDebugMenuManager::Load()
{
	const FString SaveFilePath = GetSaveFilePath();
	FString MenuString;
	if (FFileHelper::LoadFileToString(MenuString, *SaveFilePath))
	{
		if (mLoadAllMenuInstance.FromJson(MenuString))
		{
			return true;
		}
	}
	return false;
}

/**
 * @brief	指定パスのMenuNodeを構築(既にあれば生成しない)
 * @return	最終MenuNode
 */
FCSDebugMenuNodeValue* UCSDebugMenuManager::FindOrCreateMenuNode(const FString& InPath)
{
	FCSDebugMenuNodeValue& AllMenuInstance = GetAllMenuInstance();
	TArray<FString> MenuPathSplit;
	InPath.ParseIntoArray(MenuPathSplit, TEXT("/"));
	if (MenuPathSplit.Num() == 0)
	{
		return &AllMenuInstance;
	}

	FCSDebugMenuNodeValue* CurrentMenuNode = &AllMenuInstance;
	bool bNeedCreate = false;
	while (MenuPathSplit.Num() > 0)
	{
		bool bFind = false;
		for (FCSDebugMenuNodeValue& Node : CurrentMenuNode->mList)
		{
			if (Node.mDispString == MenuPathSplit[0])
			{
				Node.mbAddRuntime = true;
				CurrentMenuNode = &Node;
				MenuPathSplit.RemoveAt(0);
				bFind = true;
				break;
			}
		}

		if (!bFind)
		{
			bNeedCreate = true;
			break;
		}
	}

	if (!bNeedCreate)
	{
		return CurrentMenuNode;
	}

	while (MenuPathSplit.Num() > 0)
	{
		FCSDebugMenuNodeValue AddMenuNode;
		AddMenuNode.SetDispString(MenuPathSplit[0]);
		AddMenuNode.mbAddRuntime = true;
		CurrentMenuNode->AddMenuList(AddMenuNode);
		CurrentMenuNode = CurrentMenuNode->GetCurrentLastNode();
		MenuPathSplit.RemoveAt(0);
	}
	return CurrentMenuNode;
}

/**
 * @brief	Saveボタン
 */
void UCSDebugMenuManager::OnPushSaveButton(const FCSDebugMenuNodeGetter& InGetter)
{
	Save();
}

/**
 * @brief	Windowsのデバイス情報表示
 */
void UCSDebugMenuManager::DebugDrawPlatformInfo(UCanvas* InCanvas)
{
#if PLATFORM_WINDOWS
	FCSDebugInfoWindowText InfoWindow;
	InfoWindow.AddText(FString::Printf(TEXT("CPU : %s"), *FWindowsPlatformMisc::GetCPUBrand()));
	InfoWindow.AddText(FString::Printf(TEXT("NumberOfCores : %d"), FWindowsPlatformMisc::NumberOfCores()));
	InfoWindow.AddText(FString::Printf(TEXT("GPUBrand : %s"), *FWindowsPlatformMisc::GetPrimaryGPUBrand()));
	InfoWindow.AddText(FString::Printf(TEXT("OSVersion : %s"), *FWindowsPlatformMisc::GetOSVersion()));
	{
		FString Path = FPaths::ConvertRelativePathToFull(FPaths::RootDir());
		uint64 TotalByte;
		uint64 FreeByte;
		FWindowsPlatformMisc::GetDiskTotalAndFreeSpace(Path, TotalByte, FreeByte);
		const uint64 TotalKB = TotalByte / 1024;
		const uint64 FreeKB = FreeByte / 1024;
		const uint64 TotalMB = TotalKB / 1024;
		const uint64 FreeMB = FreeKB / 1024;
		const uint64 TotalGB = TotalMB / 1024;
		const uint64 FreeGB = FreeMB / 1024;
		InfoWindow.AddText(FString::Printf(TEXT("DiskSpace : %lluGB / %lluGB (%s)")
			, FreeGB, TotalGB, *Path)
		);
	}

	InfoWindow.Draw(InCanvas, FVector2D(50.f, 50.f));
#endif
}

//EUW用に
#if WITH_EDITOR
/**
 * @brief	Saveファイルのパス
 */
FString	UCSDebugMenuManager::EUW_GetSaveFilePath()
{
	FString SaveFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	SaveFilePath += FString(TEXT("/CSDebug/DebugMenu.json"));
	return SaveFilePath;
}
FString	UCSDebugMenuManager::EUW_GetTempSaveFilePath()
{
	FString SaveFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	SaveFilePath += FString(TEXT("/CSDebug/DebugMenuTemp.json"));
	return SaveFilePath;
}
/**
 * @brief	EUW用にFCSDebugMenuNodeValueをロード
 */
bool UCSDebugMenuManager::EUW_LoadDebugMenu(TArray<FCSDebugMenuNodeEUW>& OutData, const FString& InPath)
{
	const FString SaveFilePath = EUW_GetTempSaveFilePath();//Tempは準備済みのはず
	FString MenuString;
	if (!FFileHelper::LoadFileToString(MenuString, *SaveFilePath))
	{
		return false;
	}
	
	FCSDebugMenuNodeValue AllMenuInstance;
	if (!AllMenuInstance.FromJson(MenuString))
	{
		return false;
	}

	TArray<FString> MenuPathSplit;
	InPath.ParseIntoArray(MenuPathSplit, TEXT("/"));
	if (MenuPathSplit.Num() == 0)
	{
		for (const FCSDebugMenuNodeValue& Node : AllMenuInstance.mList)
		{
			FCSDebugMenuNodeEUW NodeEUW(Node);
			OutData.Add(NodeEUW);
		}
		return true;
	}

	const FCSDebugMenuNodeValue* FindNode = &AllMenuInstance;
	while (MenuPathSplit.Num() > 0)
	{
		bool bFind = false;
		for (const FCSDebugMenuNodeValue& Node : FindNode->mList)
		{
			if (Node.mDispString == MenuPathSplit[0])
			{
				FindNode = &Node;
				MenuPathSplit.RemoveAt(0);
				bFind = true;
				break;
			}
		}

		if (!bFind)
		{
			return false;
		}
	}

	if (FindNode)
	{
		for (const FCSDebugMenuNodeValue& Node : FindNode->mList)
		{
			FCSDebugMenuNodeEUW NodeEUW(Node);
			OutData.Add(NodeEUW);
		}
		return true;
	}
	return false;
}

/**
 * @brief	EUW用にFCSDebugMenuNodeValueを仮セーブ
 */
bool UCSDebugMenuManager::EUW_SaveDebugMenuTemp(const TArray<FCSDebugMenuNodeEUW>& InData, const FString& InPath)
{
	const FString TempSaveFilePath = EUW_GetTempSaveFilePath();
	FString MenuString;
	if (!FFileHelper::LoadFileToString(MenuString, *TempSaveFilePath))
	{
		return false;
	}

	FCSDebugMenuNodeValue AllMenuInstance;
	if (!AllMenuInstance.FromJson(MenuString))
	{
		return false;
	}

	FCSDebugMenuNodeValue* OverrideMenuNode = &AllMenuInstance;
	TArray<FString> MenuPathSplit;
	InPath.ParseIntoArray(MenuPathSplit, TEXT("/"));
	if (MenuPathSplit.Num() > 0)
	{
		FCSDebugMenuNodeValue* FindNode = &AllMenuInstance;
		while (MenuPathSplit.Num() > 0)
		{
			bool bFind = false;
			for (FCSDebugMenuNodeValue& Node : FindNode->mList)
			{
				if (Node.mDispString == MenuPathSplit[0])
				{
					FindNode = &Node;
					MenuPathSplit.RemoveAt(0);
					bFind = true;
					break;
				}
			}

			if (!bFind)
			{
				return false;
			}
		}

		if (FindNode == nullptr)
		{
			return false;
		}
		OverrideMenuNode = FindNode;
	}

	if (OverrideMenuNode == nullptr)
	{
		return false;
	}

	OverrideMenuNode->mList.Empty();
	for (const FCSDebugMenuNodeEUW& NodeEUW : InData)
	{
		FCSDebugMenuNodeValue NodeValue;
		NodeEUW.GetNodeValue(NodeValue);
		OverrideMenuNode->mList.Add(NodeValue);
	}

	return FFileHelper::SaveStringToFile(AllMenuInstance.ToJson(), *TempSaveFilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}

/**
 * @brief	EUW用にDebugMenu情報を仮セーブにコピー
 */
bool UCSDebugMenuManager::EUW_CopyDebugMenuToTemp()
{
	const FString SaveFilePath = EUW_GetSaveFilePath();
	FString MenuString;
	if (FFileHelper::LoadFileToString(MenuString, *SaveFilePath))
	{
		const FString TempSaveFilePath = EUW_GetTempSaveFilePath();
		return FFileHelper::SaveStringToFile(MenuString, *TempSaveFilePath, FFileHelper::EEncodingOptions::ForceUTF8);
	}
	return false;
}
/**
 * @brief	EUW用に仮セーブを本セーブにコピー
 */
bool UCSDebugMenuManager::EUW_CopyTempToDebugMenu()
{
	const FString TempSaveFilePath = EUW_GetTempSaveFilePath();
	FString MenuString;
	if (FFileHelper::LoadFileToString(MenuString, *TempSaveFilePath))
	{
		const FString SaveFilePath = EUW_GetSaveFilePath();
		return FFileHelper::SaveStringToFile(MenuString, *SaveFilePath, FFileHelper::EEncodingOptions::ForceUTF8);
	}
	return false;
}
#endif // WITH_EDITOR