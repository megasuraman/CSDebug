// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMenuListObjectBase.h
 * @brief DebugMenu用WidgetのListViewに使うObjectクラス
 * @author SensyuGames
 * @date 2020/08/21
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebugMenuNodeData.h"
#include "CSDebugMenuNodeGetter.h"
#include "CSDebugMenuListObjectBase.generated.h"

class UCSDebugMenuWidgetBase;
class UCSDebugMenuWidgetNodeBase;

/**
 * DebugMenu用WidgetのListViewに使うObjectクラス
 */
UCLASS(Blueprintable)
class CSDEBUG_API UCSDebugMenuListObjectBase : public UObject
{
	GENERATED_BODY()
	
public:
	void	Init(const FCSDebugMenuNodeValue& InNodeValue);
	void	OnDecide();

	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	SetNodeName(FString InString){mNodeName=InString;}
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	SetDebugMenuWidget(UCSDebugMenuWidgetBase* InWidget){mDebugMenuWidget=InWidget;}
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	SetDebugMenuWidgetNode(UCSDebugMenuWidgetNodeBase* InWidget){mDebugMenuWidgetNode=InWidget;}
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	SetListViewIndex(int32 InIndex){mListViewIndex=InIndex;}
	void	SetMenuPath(const FString& InPath) { mMenuPath = InPath; }

	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	int32	GetListViewIndex() const {return mListViewIndex;}
	const FCSDebugMenuNodeValue& GetNodeValue() const { return mNodeValue; }
	const FString& GetMenuPath() const { return mMenuPath; }

protected:
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	FString	GetNodeName() const {return mNodeName;}
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	FString	GetValueName() const {return mValueName;}
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	TArray<FString>	GetSelectorItemList() const;
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	int32	GetSelectorIndex() const;
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	UCSDebugMenuWidgetBase*	GetDebugMenuWidget() const {return mDebugMenuWidget.Get();}
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	UCSDebugMenuWidgetNodeBase*	GetDebugMenuWidgetNode() const {return mDebugMenuWidgetNode.Get();}
	
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	OnSelect();
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	OnNoSelect();
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	OnCancel();

	UFUNCTION(BlueprintImplementableEvent)
	void	OnSelectBP();
	UFUNCTION(BlueprintImplementableEvent)
	void	OnNoSelectBP();
	UFUNCTION(BlueprintImplementableEvent)
	void	OnDecideBP();
	UFUNCTION(BlueprintImplementableEvent)
	void	OnSetValueNameBP();

	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	SetValueBool(bool bInBool);
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	SetValueInt(int32 InInt);
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	SetValueFloat(float InFloat);
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	SetValueSelectorListIndex(int32 InListIndex);

	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	SetValueName(const FString& InString);
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	ECSDebugMenuNodeValueKind	GetNodeVaueKind() const;
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	SetFromEUW(bool bInFromEUW)
	{
		mNodeValue.mbFromEUW = bInFromEUW;
	}
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	bool	IsFromEUW() const{return mNodeValue.mbFromEUW;}

private:
	FCSDebugMenuNodeValue	mNodeValue;
	FString		mNodeName;
	FString		mValueName;
	FString		mMenuPath;
	TWeakObjectPtr<UCSDebugMenuWidgetBase>	mDebugMenuWidget;
	TWeakObjectPtr<UCSDebugMenuWidgetNodeBase>	mDebugMenuWidgetNode;
	int32		mListViewIndex = 0;

	bool	mbTest = false;

#if WITH_EDITOR	// EUW用に
protected:
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	EUWSetupValue(const FCSDebugMenuNodeEUW& InData);
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	FString	EUWGetNodeValueJsonString() const;
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	EUWChangeValueKind(ECSDebugMenuNodeValueKind InKind);
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	FString	EUWGetValueString() const;
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	EUWOverrideSelector(const TArray<FString>& InSelector, const int32 InIndex);
#endif//WITH_EDITOR
};
