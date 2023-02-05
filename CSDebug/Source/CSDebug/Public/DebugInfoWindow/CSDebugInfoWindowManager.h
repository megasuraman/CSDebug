// Copyright 2020 SensyuGames.
/**
 * @file CSDebugInfoWindowManager.h
 * @brief DebugInfoWindowä«óùópÇÃManager
 * @author SensyuGames
 * @date 2023/02/04
 */
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebugInfoWindowText.h"
#include "CSDebugLoopOrderArray.h"
#include "CSDebugInfoWindowManager.generated.h"

class APlayerController;
class UCanvas;

struct CSDEBUG_API FCSDebugInfoWindowOption
{
	FLinearColor	mFrameColor = FLinearColor(0.1f, 0.9f, 0.1f, 1.f);
	float	mDispTime = -1.f;
	float	mDispBorderDistance = -1.f;
};

/**
 * 
 */
UCLASS(BlueprintType)
class CSDEBUG_API UCSDebugInfoWindowManager : public UObject
{
	GENERATED_BODY()

	UCSDebugInfoWindowManager();
	virtual void BeginDestroy() override;

public:
	static	UCSDebugInfoWindowManager* Get(UObject* InOwner);
	void	Init();
	bool	DebugTick(float InDeltaSecond);
	void	DebugDraw(UCanvas* InCanvas);

	void	AddWindow(const FName InTag, const FString& InMessage, const AActor* InFollowActor=nullptr, const FCSDebugInfoWindowOption& InOption= FCSDebugInfoWindowOption());
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	AddWindowBP(const FName InTag, const FText InMessage, const AActor* InFollowActor, float InDispTime);
protected:
	void	OnAddWindow(const FName InTag, const FString& InMessage, const AActor* InFollowActor, const FCSDebugInfoWindowOption& InOption);

	void	UpdateLifeTime(const float InDeltaSecond);

	void	DrawWindow(UCanvas* InCanvas);

private:
	struct FTempWindowData
	{
		FCSDebugInfoWindowText	mWindow;
		FName	mTagName;
		TWeakObjectPtr<const AActor>	mFollowTarget;
		float	mLifeTime = -1.f;
		bool	mbActive = false;
	};
	TArray<FTempWindowData>	mTempWindowDataList;
};
