// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMenuWidgetBase.cpp
 * @brief CSDebugMenuのWidget管理クラス
 * @author SensyuGames
 * @date 2020/08/01
 */

#include "DebugMenu/CSDebugMenuWidgetBase.h"
#include "Components/ListView.h"
#include "CSDebugConfig.h"
#include "DebugMenu/CSDebugMenuListObjectBase.h"
#include "DebugMenu/CSDebugMenuManager.h"

/**
 * @brief	Init
 */
void	UCSDebugMenuWidgetBase::Init(UCSDebugMenuManager* InManager)
{
	mManager = InManager;
	UListView* ListView = mListView.Get();
	if (ListView == nullptr)
	{
		return;
	}

	mMenuPath = InManager->GetCurrentMenuPath();
	OnChangedMenuPathBP();
	ChangeMenu(mMenuPath);
	mInputMode = ECSDebugMenuInputMode::Menu;
}

/**
 * @brief	Tick
 */
bool	UCSDebugMenuWidgetBase::DebugTick(const UPlayerInput& InInput, const float InDeltaSecond)
{
	const UCSDebugConfig* CSDebugConfig = GetDefault<UCSDebugConfig>();
	if (CSDebugConfig->mDebugCommand_ReadyKey.IsPressed(&InInput))
	{
		return false;
	}

	bool bAction = false;
	if (CSDebugConfig->mDebugMenu_SelectKey.IsJustPressed(&InInput))
	{
		OnPushDecide();
		bAction = true;
	}
	else if (CSDebugConfig->mDebugMenu_CancelKey.IsJustPressed(&InInput))
	{
		OnPushCancel();
		bAction = true;
	}
	else if (CSDebugConfig->mDebugMenu_UpKey.IsJustPressed(&InInput))
	{
		OnPushUp();
		bAction = true;
	}
	else if (CSDebugConfig->mDebugMenu_DownKey.IsJustPressed(&InInput))
	{
		OnPushDown();
		bAction = true;
	}
	else if (CSDebugConfig->mDebugMenu_LeftKey.IsJustPressed(&InInput))
	{
		OnPushLeft();
		bAction = true;
	}
	else if (CSDebugConfig->mDebugMenu_RightKey.IsJustPressed(&InInput))
	{
		OnPushRight();
		bAction = true;
	}

	if (mbRequestChangeNextMenu)
	{
		if(const FCSDebugMenuNodeValue* RunMenu = mManager->FindNode(mMenuPath))
		{
			//RunMenu->mSelectIndex = mSelectIndex;
			mMenuPath += FString::Printf(TEXT("/%s"), *RunMenu->mList[mSelectIndex].mDispString);
			OnChangedMenuPathBP();
			ChangeMenu(mMenuPath);
		}
		mbRequestChangeNextMenu = false;
	}
	else if (mbRequestChangeOldMenu)
	{
		if (const FCSDebugMenuNodeValue* RunMenu = mManager->FindNode(mMenuPath))
		{

			//RunMenu->mSelectIndex = mSelectIndex;
		}

		FString PreMenuPath;
		FString TempPath;
		mMenuPath.Split(TEXT("/"), &PreMenuPath, &TempPath, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
		mMenuPath = PreMenuPath;
		OnChangedMenuPathBP();
		ChangeMenu(mMenuPath);
		mbRequestChangeOldMenu = false;
	}

	return bAction;
}

/**
 * @brief	メニュー切り替えリクエスト
 */
void	UCSDebugMenuWidgetBase::RequestChangeNextMenu()
{
	mbRequestChangeNextMenu = true;
}
/**
 * @brief	整数値編集開始リクエスト
 */
void	UCSDebugMenuWidgetBase::RequestBeginNodeInt()
{
	SetInputMode(ECSDebugMenuInputMode::Int);
}
/**
 * @brief	小数値編集開始リクエスト
 */
void	UCSDebugMenuWidgetBase::RequestBeginNodeFloat()
{
	SetInputMode(ECSDebugMenuInputMode::Float);
}
/**
 * @brief	リスト編集開始リクエスト
 */
void	UCSDebugMenuWidgetBase::RequestBeginNodeSelector()
{
	SetInputMode(ECSDebugMenuInputMode::Selector);
}
/**
 * @brief	ボタン押し
 */
void	UCSDebugMenuWidgetBase::RequestPushButton()
{
	SetInputMode(ECSDebugMenuInputMode::Invalid);
	OnPushDecide();
}

/**
 * @brief	初期化処理
 */
void	UCSDebugMenuWidgetBase::InitBP(UListView* InListView, int32 InDispNodeNum)
{
	mListView = InListView;
	mListViewDispNodeNum = InDispNodeNum;
}

/**
 * @brief	選択移動
 */
void	UCSDebugMenuWidgetBase::MoveSelect(int32 InOffsetIndex)
{
	const UListView* ListView = mListView.Get();
	if (ListView == nullptr)
	{
		return;
	}
	const int32 ListItemNum = ListView->GetNumItems();
	if (ListItemNum <= 0)
	{
		return;
	}
	const int32 OldIndex = mSelectIndex;
	mSelectIndex = FMath::Clamp(mSelectIndex+InOffsetIndex, 0, ListItemNum - 1);
	if (OldIndex != mSelectIndex)
	{
		OnChangedSelectBP(OldIndex);
		if (!mbNoCheckScrollOnMoveSelect)
		{
			CheckListViewScroll();
		}
	}
}

/**
 * @brief	スクロールチェック
 */
void	UCSDebugMenuWidgetBase::CheckListViewScroll()
{
	UListView* ListView = mListView.Get();
	if (ListView == nullptr)
	{
		return;
	}
	const int32 OldScrollCount = mScrollCount;
	const int32 ListItemNum = ListView->GetNumItems();
	if (mSelectIndex + 2 >= mScrollCount + mListViewDispNodeNum)
	{
		mScrollCount = FMath::Min(mScrollCount + 1, ListItemNum);
	}
	else if (mSelectIndex - 1 <= mScrollCount)
	{
		mScrollCount = FMath::Max(mScrollCount - 1, 0);
	}

	if (OldScrollCount != mScrollCount)
	{
		ListView->SetScrollOffset((float)mScrollCount);
	}
}

/**
 * @brief	決定時処理
 */
void	UCSDebugMenuWidgetBase::OnPushDecide()
{
	if (mInputMode != ECSDebugMenuInputMode::Menu)
	{
		OnPushDecideBP();
		SetInputMode(ECSDebugMenuInputMode::Menu);
		ReflectDebugMenuNodeObject();
		return;
	}

	UListView* ListView = mListView.Get();
	if (ListView == nullptr)
	{
		return;
	}
	UCSDebugMenuListObjectBase* SelectNodeObject = Cast<UCSDebugMenuListObjectBase>( ListView->GetItemAt(mSelectIndex) );
	if (SelectNodeObject == nullptr)
	{
		return;
	}

	SelectNodeObject->OnDecide();
	ReflectDebugMenuNodeObject();
}
/**
 * @brief	キャンセル時処理
 */
void	UCSDebugMenuWidgetBase::OnPushCancel()
{
	if (mInputMode != ECSDebugMenuInputMode::Menu)
	{
		OnPushCancelBP();
		SetInputMode(ECSDebugMenuInputMode::Menu);
		return;
	}
	if (const FCSDebugMenuNodeValue* RunMenu = mManager->FindNode(mMenuPath))
	{
		if (RunMenu == &mManager->GetAllMenuInstance())
		{
			return;
		}
	}
	mbRequestChangeOldMenu = true;
}
/**
 * @brief	上移動時処理
 */
void	UCSDebugMenuWidgetBase::OnPushUp()
{
	if (mInputMode != ECSDebugMenuInputMode::Menu)
	{
		OnPushUpBP();
		return;
	}
	MoveSelect(-1);
}
/**
 * @brief	下移動時処理
 */
void	UCSDebugMenuWidgetBase::OnPushDown()
{
	if (mInputMode != ECSDebugMenuInputMode::Menu)
	{
		OnPushDownBP();
		return;
	}
	MoveSelect(1);
}
/**
 * @brief	左移動時処理
 */
void	UCSDebugMenuWidgetBase::OnPushLeft()
{
	if (mInputMode != ECSDebugMenuInputMode::Menu)
	{
		OnPushLeftBP();
	}
	else
	{
		OnPushCancel();
	}
}
/**
 * @brief	右移動時処理
 */
void	UCSDebugMenuWidgetBase::OnPushRight()
{
	if (mInputMode != ECSDebugMenuInputMode::Menu)
	{
		OnPushRightBP();
	}
	else
	{
		OnPushDecide();
	}
}

/**
 * @brief	指定パスのメニューに切り替える
 */
void	UCSDebugMenuWidgetBase::ChangeMenu(const FString& InPath)
{
	const FCSDebugMenuNodeValue* MenuNode = mManager->FindNode(InPath);
	if (MenuNode == nullptr)
	{
		return;
	}

	UListView* ListView = mListView.Get();
	if (ListView == nullptr)
	{
		return;
	}

	//ListViewが持つNodeObjectの値を反映
	for (const UObject* ListViewObject : ListView->GetListItems())
	{
		if (const UCSDebugMenuListObjectBase* NodeObject = Cast<UCSDebugMenuListObjectBase>(ListViewObject))
		{
			ReflectDebugMenuNodeObject(NodeObject);
		}
	}

	ListView->ClearListItems();
	mSelectIndex = 0;
	mScrollCount = 0;

	const UCSDebugConfig* CSDebugConfig = GetDefault<UCSDebugConfig>();
	for (int32 i = 0; i < MenuNode->mList.Num(); ++i)
	{
		UCSDebugMenuListObjectBase* NodeObject = NewObject<UCSDebugMenuListObjectBase>(this, CSDebugConfig->mDebugMenu_ListObjectClass);
		if (NodeObject == nullptr)
		{
			return;
		}
		NodeObject->Init(MenuNode->mList[i]);
		NodeObject->SetDebugMenuWidget(this);
		NodeObject->SetListViewIndex(i);
		NodeObject->SetMenuPath(InPath);
		ListView->AddItem(NodeObject);
	}
	ListView->SetSelectedIndex(mSelectIndex);
	OnChangedSelectBP(0);
	mManager->SetCurrentMenuPath(InPath);
}
/**
 * @brief	表示中のオブジェクトの値をmAllMenueInstanceに反映させる
 */
void	UCSDebugMenuWidgetBase::ReflectDebugMenuNodeObject()
{
	const FCSDebugMenuNodeValue* MenuNode = mManager->FindNode(mMenuPath);
	if (MenuNode == nullptr)
	{
		return;
	}

	UListView* ListView = mListView.Get();
	if (ListView == nullptr)
	{
		return;
	}

	//ListViewが持つNodeObjectの値を反映
	for (const UObject* ListViewObject : ListView->GetListItems())
	{
		if (const UCSDebugMenuListObjectBase* NodeObject = Cast<UCSDebugMenuListObjectBase>(ListViewObject))
		{
			ReflectDebugMenuNodeObject(NodeObject);
		}
	}
}
void	UCSDebugMenuWidgetBase::ReflectDebugMenuNodeObject(const UCSDebugMenuListObjectBase* InObject)
{
	if (InObject == nullptr)
	{
		return;
	}
	if (InObject->GetNodeValue().GetKind() == ECSDebugMenuNodeValueKind::Menu)
	{
		return;
	}
	FCSDebugMenuNodeValue* MenuNode = mManager->FindNode(InObject->GetMenuPath());
	if (MenuNode == nullptr)
	{
		return;
	}
	if (InObject->GetListViewIndex() < MenuNode->mList.Num())
	{
		FCSDebugMenuNodeValue& NodeValue = MenuNode->mList[InObject->GetListViewIndex()];
		NodeValue = InObject->GetNodeValue();
	}
}

/**
 * @brief	
 */
void	UCSDebugMenuWidgetBase::SetInputMode(const ECSDebugMenuInputMode InMode)
{
	if (mInputMode == InMode)
	{
		return;
	}

	switch (mInputMode)
	{
	case ECSDebugMenuInputMode::Int:
		OnEndNodeIntBP();
		break;
	case ECSDebugMenuInputMode::Float:
		OnEndNodeFloatBP();
		break;
	case ECSDebugMenuInputMode::Selector:
		OnEndNodeSelectorBP();
		break;
	default:
		break;
	}

	mInputMode = InMode;

	switch (mInputMode)
	{
	case ECSDebugMenuInputMode::Int:
		OnBeginNodeIntBP();
		break;
	case ECSDebugMenuInputMode::Float:
		OnBeginNodeFloatBP();
		break;
	case ECSDebugMenuInputMode::Selector:
		OnBeginNodeSelectorBP();
		break;
	default:
		break;
	}
}