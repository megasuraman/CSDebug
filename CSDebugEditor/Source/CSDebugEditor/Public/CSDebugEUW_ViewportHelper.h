// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "CSDebugEUW_ViewportHelper.generated.h"

/**
 * 
 */
UCLASS()
class CSDEBUGEDITOR_API UCSDebugEUW_ViewportHelper : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void	LookAt(const FString& InString);
	
	UFUNCTION(BlueprintCallable)
	void	RequestDraw(bool bInDraw);

protected:
	virtual void NativeDestruct() override;

	void	Draw(UCanvas* InCanvas, APlayerController* InPlayerController);

private:
	FVector		mLastLookAtLocation = FVector::ZeroVector;
	FDelegateHandle	mDebugDrawHandle;
};
