// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMenuWidgetBase.h
 * @brief CSDebugMenuのWidget管理クラス
 * @author SensyuGames
 * @date 2020/08/01
 */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSDebugMenuNodeData.h"
#include "CSDebugMenuWidgetBase.generated.h"

class UPlayerInput;
class UListView;
class UCSDebugMenuListObjectBase;
class UCSDebugMenuManager;

UENUM(BlueprintType)
enum class ECSDebugMenuInputMode : uint8
{
	Invalid,
	Menu,
	Int,
	Float,
	Selector,
};

/**
 * DebugMenu用Widgetクラス
 */
UCLASS()
class CSDEBUG_API UCSDebugMenuWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void	Init(UCSDebugMenuManager* InManager);
	bool	DebugTick(const UPlayerInput& InInput, const float InDeltaSecond);
	void	RequestChangeNextMenu();
	void	RequestBeginNodeInt();
	void	RequestBeginNodeFloat();
	void	RequestBeginNodeSelector();
	void	RequestPushButton();

protected:
	UFUNCTION(BlueprintCallable)
	void	InitBP(UListView* InListView, int32 InDispNodeNum);
	UFUNCTION(BlueprintCallable)
	void	MoveSelect(int32 InOffsetIndex);
	UFUNCTION(BlueprintCallable)
	void	SetSelectIndexInvalid()
	{
		mSelectIndex = -1;
	}
	UFUNCTION(BlueprintImplementableEvent)
	void	OnChangedSelectBP(const int32 InOldIndex);
	UFUNCTION(BlueprintCallable)
	int32	GetSelectIndex() const { return mSelectIndex; }
	UFUNCTION(BlueprintCallable)
	int32	GetLisetViewDispNodeNum() const { return mListViewDispNodeNum; }
	UFUNCTION(BlueprintImplementableEvent)
	void	OnChangedMenuPathBP();
	UFUNCTION(BlueprintCallable)
	FString	GetMenuPath() const { return mMenuPath; }
	UFUNCTION(BlueprintImplementableEvent)
	void	OnBeginNodeIntBP();
	UFUNCTION(BlueprintImplementableEvent)
	void	OnEndNodeIntBP();
	UFUNCTION(BlueprintImplementableEvent)
	void	OnBeginNodeFloatBP();
	UFUNCTION(BlueprintImplementableEvent)
	void	OnEndNodeFloatBP();
	UFUNCTION(BlueprintImplementableEvent)
	void	OnBeginNodeSelectorBP();
	UFUNCTION(BlueprintImplementableEvent)
	void	OnEndNodeSelectorBP();
	UFUNCTION(BlueprintImplementableEvent)
	void	OnPushDecideBP();
	UFUNCTION(BlueprintImplementableEvent)
	void	OnPushCancelBP();
	UFUNCTION(BlueprintImplementableEvent)
	void	OnPushUpBP();
	UFUNCTION(BlueprintImplementableEvent)
	void	OnPushDownBP();
	UFUNCTION(BlueprintImplementableEvent)
	void	OnPushLeftBP();
	UFUNCTION(BlueprintImplementableEvent)
	void	OnPushRightBP();
	UFUNCTION(BlueprintCallable)
	ECSDebugMenuInputMode	GetInputMode() const { return mInputMode; }
	UFUNCTION(BlueprintCallable)
	void	SetNoCheckScrollOnMoveSelect(bool InNoCheck)
	{
		mbNoCheckScrollOnMoveSelect = InNoCheck;
	}

protected:
	void	CheckListViewScroll();
	void	OnPushDecide();
	void	OnPushCancel();
	void	OnPushUp();
	void	OnPushDown();
	void	OnPushLeft();
	void	OnPushRight();
	void	ChangeMenu(const FString& InPath);
	void	ReflectDebugMenuNodeObject();
	void	ReflectDebugMenuNodeObject(const UCSDebugMenuListObjectBase* InObject);
	void	SetInputMode(const ECSDebugMenuInputMode InMode);


private:
	TWeakObjectPtr<UCSDebugMenuManager>	mManager;
	TWeakObjectPtr<UListView>	mListView;
	FString	mMenuPath;
	int32	mListViewDispNodeNum = 10;
	int32	mSelectIndex = -1;
	int32	mScrollCount = 0;
	ECSDebugMenuInputMode	mInputMode = ECSDebugMenuInputMode::Invalid;
	bool	mbRequestChangeNextMenu = false;
	bool	mbRequestChangeOldMenu = false;
	bool	mbNoCheckScrollOnMoveSelect = false;
};
