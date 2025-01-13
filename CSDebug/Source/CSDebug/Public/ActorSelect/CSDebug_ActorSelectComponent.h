// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_ActorSelectComponent.h
 * @brief デバッグ選択対象Component
 * @author SensyuGames
 * @date 2020/05/27
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScreenWindow/CSDebug_ScreenWindowText.h"
#include "CSDebug_ActorSelectComponent.generated.h"

class UCSDebug_ActorSelectManager;
class UCanvas;
class AAIController;

DECLARE_DYNAMIC_DELEGATE(FDebugSelectPreDrawDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CSDEBUG_API UCSDebug_ActorSelectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCSDebug_ActorSelectComponent();

	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void    AddTextBP(const FString& InString)
	{
#if USE_CSDEBUG
		mScreenWindow.AddText(InString);
#endif//USE_CSDEBUG
	}
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void    AddPreDrawDelegateBP(FDebugSelectPreDrawDelegate InDelegate)
	{
#if USE_CSDEBUG
		mPreDrawDelegate = InDelegate;
		mbUsePreDrawDelegate = true;
#endif//USE_CSDEBUG
	}

protected:
	UPROPERTY(EditAnywhere)
	FLinearColor	mMyColor = FLinearColor(0.1f, 0.9f, 0.1f, 1.f);


#if USE_CSDEBUG
public:	

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	void    AddText(const FString& InString)
	{
		mScreenWindow.AddText(InString);
	}
    void    AddPreDrawDelegate(FDebugSelectPreDrawDelegate InDelegate)
    {
        mPreDrawDelegate = InDelegate;
        mbUsePreDrawDelegate = true;
    }

	void	DebugDraw(UCanvas* InCanvas);
	void	DrawMark(class UCanvas* InCanvas) const;

	void	SetSelect(bool bInSelect);
	bool	IsSelect() const { return !!mbSelect; }
	const FLinearColor&	GetMyColor() const { return mMyColor; }

private:
	FCSDebug_ScreenWindowText    mScreenWindow;
	FDebugSelectPreDrawDelegate mPreDrawDelegate;
	TWeakObjectPtr<UCSDebug_ActorSelectManager>	mManager;
	uint8	mbSelect : 1;//選択済み
	uint8   mbUsePreDrawDelegate : 1;
	uint8 : 6;

#endif//USE_CSDEBUG
};
