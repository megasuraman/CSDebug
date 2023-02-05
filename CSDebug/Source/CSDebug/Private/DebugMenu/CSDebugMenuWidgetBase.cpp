// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMenuWidgetBase.cpp
 * @brief CSDebugMenu��Widget�Ǘ��N���X
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
 * @brief	���j���[�؂�ւ����N�G�X�g
 */
void	UCSDebugMenuWidgetBase::RequestChangeNextMenu()
{
	mbRequestChangeNextMenu = true;
}
/**
 * @brief	�����l�ҏW�J�n���N�G�X�g
 */
void	UCSDebugMenuWidgetBase::RequestBeginNodeInt()
{
	SetInputMode(ECSDebugMenuInputMode::Int);
}
/**
 * @brief	�����l�ҏW�J�n���N�G�X�g
 */
void	UCSDebugMenuWidgetBase::RequestBeginNodeFloat()
{
	SetInputMode(ECSDebugMenuInputMode::Float);
}
/**
 * @brief	���X�g�ҏW�J�n���N�G�X�g
 */
void	UCSDebugMenuWidgetBase::RequestBeginNodeSelector()
{
	SetInputMode(ECSDebugMenuInputMode::Selector);
}
/**
 * @brief	�{�^������
 */
void	UCSDebugMenuWidgetBase::RequestPushButton()
{
	SetInputMode(ECSDebugMenuInputMode::Invalid);
	OnPushDecide();
}

/**
 * @brief	����������
 */
void	UCSDebugMenuWidgetBase::InitBP(UListView* InListView, int32 InDispNodeNum)
{
	mListView = InListView;
	mListViewDispNodeNum = InDispNodeNum;
}

/**
 * @brief	�I���ړ�
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
 * @brief	�X�N���[���`�F�b�N
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
 * @brief	���莞����
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
 * @brief	�L�����Z��������
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
 * @brief	��ړ�������
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
 * @brief	���ړ�������
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
 * @brief	���ړ�������
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
 * @brief	�E�ړ�������
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
 * @brief	�w��p�X�̃��j���[�ɐ؂�ւ���
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

	//ListView������NodeObject�̒l�𔽉f
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
 * @brief	�\�����̃I�u�W�F�N�g�̒l��mAllMenueInstance�ɔ��f������
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

	//ListView������NodeObject�̒l�𔽉f
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