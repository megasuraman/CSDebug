// Copyright 2022 SensyuGames.

#pragma once

#include "CoreMinimal.h"
#include "DebugMenu/CSDebug_DebugMenuTableRow.h"

class UCSDebug_DebugMenuManager;

struct FCSDebug_DebugMenuNodeActionParameter
{
	TWeakObjectPtr<APlayerController> mPlayerController;
};
DECLARE_DELEGATE_OneParam(FCSDebug_DebugMenuNodeActionDelegate, const FCSDebug_DebugMenuNodeActionParameter&);


class CSDEBUG_API CSDebug_DebugMenuNodeBase
{
public:
	CSDebug_DebugMenuNodeBase();
	virtual ~CSDebug_DebugMenuNodeBase();

	void Init(const FString& InPath, const FCSDebug_DebugMenuNodeData& InData, UCSDebug_DebugMenuManager* InManager);
	virtual void OnBeginAction();
	virtual void OnEndAction(const FCSDebug_DebugMenuNodeActionParameter& InParameter);
	virtual void OnJustPressedUpKey() {}
	virtual void OnJustPressedDownKey() {}
	virtual void OnJustPressedLeftKey() {}
	virtual void OnJustPressedRightKey() {}
	void Draw(UCanvas* InCanvas, const FVector2D& InPos, const bool bInSelect) const;
	const FString& GetPath() const{return mPath;}
	virtual FString GetValueString() const;
	bool GetBool() const;
	int32 GetInt() const;
	float GetFloat() const;
	int32 GetSelectIndex() const;
	FString GetSelectString() const;
	void SetNodeAction(const FCSDebug_DebugMenuNodeActionDelegate& InDelegate);
	const FCSDebug_DebugMenuNodeData& GetNodeData() const{return mNodeData;}

protected:
	void SetValueString(const FString& InString){mValueString = InString;}
	virtual void SetInitValue();
	void SetValueBool(const bool InValue);
	void SetValueInt(const int32 InValue);
	void SetValueFloat(const float InValue);
	void SetValueList(const int32 InSelectIndex);
	virtual void DrawValue(UCanvas* InCanvas, const FVector2D& InPos, const FLinearColor InColor) const;
	virtual void DrawEditValue(UCanvas* InCanvas, const FVector2D& InValuePos, const FVector2D& InValueExtent) const;
	bool IsEditMode() const { return mbEditMode; }
	FLinearColor GetWindowBackColor() const { return FLinearColor(0.01f, 0.01f, 0.01f, 0.5f); }
	FLinearColor GetWindowFrameColor() const { return FLinearColor(0.1f, 0.9f, 0.1f, 1.f); }
	FLinearColor GetFontColor() const { return FLinearColor(0.1f, 0.9f, 0.1f, 1.f); }
	FLinearColor GetSelectColor() const{return FLinearColor(0.1f, 0.9f, 0.9f, 1.f);}
	UCSDebug_DebugMenuManager* GetManager() const;

private:
	FCSDebug_DebugMenuNodeData mNodeData;
	FCSDebug_DebugMenuNodeActionDelegate mActionDelegate;
	FString mValueString;
	FString mPath;
	TWeakObjectPtr<UCSDebug_DebugMenuManager> mManager;
	bool mbEditMode = false;
};
