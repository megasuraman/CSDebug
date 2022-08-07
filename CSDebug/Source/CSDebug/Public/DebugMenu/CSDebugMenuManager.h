// Copyright 2020 SensyuGames.
/**
 * @file CSDebugMenuManager.h
 * @brief CSDebugMenuのManagerクラス
 * @author SensyuGames
 * @date 2020/08/01
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebugMenuNodeGetter.h"
#include "CSDebugMenuManager.generated.h"


class APlayerController;
class UCSDebugMenuWidgetBase;
class UCanvas;
struct FCSDebugMenuNodeValue;
struct FCSDebugMenuNodeValueBP;

/**
 * 
 */
UCLASS()
class CSDEBUG_API UCSDebugMenuManager : public UObject
{
	GENERATED_BODY()

	UCSDebugMenuManager();
	virtual void BeginDestroy() override;
	
public:
#if USE_CSDEBUG
	static	UCSDebugMenuManager* Get(UObject* InOwner);
#endif //USE_CSDEBUG
	void	Init();
	bool	DebugTick(float InDeltaSecond);
	void	DebugDraw(UCanvas* InCanvas);

	void	SetActive(const bool bInActive);
	void	SetChangeValueCallback(const FString& InPath, const FCSDebugMenuNodeDelegate& InDelegate);

	//メンバ変数を連動させるためのラッパー
#define AddNodePropertyBool(_PathString_, _Value_)\
	AddValueNode(_PathString_, _Value_,\
		FCSDebugMenuNodeDelegate::CreateLambda([&](const FCSDebugMenuNodeGetter& InGetter){\
			_Value_ = InGetter.GetBool();}))
#define AddNodePropertyInt(_PathString_, _Value_)\
	AddValueNode(_PathString_, _Value_,\
		FCSDebugMenuNodeDelegate::CreateLambda([&](const FCSDebugMenuNodeGetter& InGetter){\
			_Value_ = InGetter.GetValueInt();}))
#define AddNodePropertyFloat(_PathString_, _Value_)\
	AddValueNode(_PathString_, _Value_,\
		FCSDebugMenuNodeDelegate::CreateLambda([&](const FCSDebugMenuNodeGetter& InGetter){\
			_Value_ = InGetter.GetValueFloat();}))
#define AddNodePropertyListIndex(_PathString_, _List_, _SelectIndex_)\
	AddValueNode(_PathString_, _List_, _SelectIndex_,\
		FCSDebugMenuNodeDelegate::CreateLambda([&](const FCSDebugMenuNodeGetter& InGetter){\
			_SelectIndex_ = InGetter.GetSelectIndex();}))

	//Delegate付きでノード追加
	void	AddValueNode(const FString& InPathNode, const bool bInInitValue, const FCSDebugMenuNodeDelegate& InDelegate = nullptr);
	void	AddValueNode(const FString& InPathNode, const int32 InInitValue, const FCSDebugMenuNodeDelegate& InDelegate = nullptr);
	void	AddValueNode(const FString& InPathNode, const float InInitValue, const FCSDebugMenuNodeDelegate& InDelegate = nullptr);
	void	AddValueNode(const FString& InPathNode, const TArray<FString>& InList, const int32 InSelectIndex, const FCSDebugMenuNodeDelegate& InDelegate = nullptr);
	void	AddButtonNode(const FString& InPathNode, const FCSDebugMenuNodeDelegate& InDelegate);
	
	bool	IsActive() const { return mbActive; }
	FCSDebugMenuNodeValue& GetAllMenuInstance() { return mAllMenuInstance; }
	const FCSDebugMenuNodeValue& GetAllMenuInstance() const { return mAllMenuInstance; }
	FCSDebugMenuNodeValue* FindNode(const FString& InPath);
	const FCSDebugMenuNodeValue* FindNode(const FString& InPath) const;
	void	SetCurrentMenuPath(const FString& InPath) { mCurrentMenuPath = InPath; }
	const FString& GetCurrentMenuPath() const { return mCurrentMenuPath; }

	UFUNCTION(BlueprintCallable, meta =(DevelopmentOnly, Category = "CSDebug"))
	bool	GetValueBool(const FString& InPath) const;
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	int32	GetValueInt(const FString& InPath) const;
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	float	GetValueFloat(const FString& InPath) const;
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	int32	GetValueSelectIndex(const FString& InPath) const;
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	FString	GetValueSelectString(const FString& InPath) const;

protected:
	APlayerController* FindPlayerController() const;
	void	CreateMenuWidget();
	void	DeleteMenuWidget();

	FString	GetSaveFilePath() const;
	bool	Save();
	bool	Load();

	FCSDebugMenuNodeValue* FindOrCreateMenuNode(const FString& InPath);

	void	OnPushSaveButton(const FCSDebugMenuNodeGetter& InGetter);
	void	DebugDrawPlatformInfo(UCanvas* InCanvas);

protected:
	UPROPERTY()
	UCSDebugMenuWidgetBase*	mMenuWidget = nullptr;
	FCSDebugMenuNodeValue		mAllMenuInstance;
	FCSDebugMenuNodeValue		mLoadAllMenuInstance;
	FString	mCurrentMenuPath;
	bool	mbDispPlatformInfo = false;
	uint8	mbActive : 1;
	uint8 : 7;

	//EUW用に
#if WITH_EDITOR
protected:
	static FString	EUW_GetSaveFilePath();
	static FString	EUW_GetTempSaveFilePath();
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	static bool	EUW_LoadDebugMenu(TArray<FCSDebugMenuNodeEUW>& OutData, const FString& InPath);
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	static bool	EUW_SaveDebugMenuTemp(const TArray<FCSDebugMenuNodeEUW>& InData, const FString& InPath);
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	static bool	EUW_CopyDebugMenuToTemp();
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	static bool	EUW_CopyTempToDebugMenu();
#endif
};
