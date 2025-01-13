// Copyright 2020 SensyuGames.
/**
 * @file CSDebug_ScreenWindowManager.h
 * @brief DebugInfoWindowä«óùópÇÃManager
 * @author SensyuGames
 * @date 2023/02/04
 */
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSDebug_ScreenWindowText.h"
#include "CSDebug_ScreenWindowManager.generated.h"

class APlayerController;
class UCanvas;

struct CSDEBUG_API FCSDebug_ScreenWindowOption
{
	FLinearColor	mFrameColor = FLinearColor(0.1f, 0.9f, 0.1f, 1.f);
	float	mDispTime = -1.f;
	float	mDispBorderDistance = -1.f;
};

/**
 * 
 */
UCLASS(BlueprintType)
class CSDEBUG_API UCSDebug_ScreenWindowManager : public UObject
{
	GENERATED_BODY()

public:
	UCSDebug_ScreenWindowManager();

	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSDebug"))
	void	AddWindowBP(const FName InTag, const FText InMessage, const AActor* InFollowActor, float InDispTime);

#if USE_CSDEBUG
public:
	static	UCSDebug_ScreenWindowManager* Get(UObject* InOwner);
	void	Init();
	bool	DebugTick(float InDeltaSecond);
	void	DebugDraw(UCanvas* InCanvas);

	void	AddWindow(const FName InTag, const FString& InMessage, const AActor* InFollowActor=nullptr, const FCSDebug_ScreenWindowOption& InOption= FCSDebug_ScreenWindowOption());

protected:
	void	OnAddWindow(const FName InTag, const FString& InMessage, const AActor* InFollowActor, const FCSDebug_ScreenWindowOption& InOption);

	void	UpdateLifeTime(const float InDeltaSecond);

	void	DrawWindow(UCanvas* InCanvas);

private:
	struct FTempWindowData
	{
		FCSDebug_ScreenWindowText	mWindow;
		FName	mTagName;
		TWeakObjectPtr<const AActor>	mFollowTarget;
		float	mLifeTime = -1.f;
		bool	mbActive = false;
	};
	TArray<FTempWindowData>	mTempWindowDataList;
#endif//USE_CSDEBUG
};
