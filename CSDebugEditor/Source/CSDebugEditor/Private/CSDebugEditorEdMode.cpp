// Copyright 2020 SensyuGames.
/**
 * @file CSDebugEditorEdMode.cpp
 * @brief CSDebugEditorEdMode
 * @author SensyuGames
 * @date 2020/7/24
 */

#include "CSDebugEditorEdMode.h"
#include "CSDebugEditorEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

const FEditorModeID FCSDebugEditorEdMode::EM_CSDebugEditorEdModeId = TEXT("EM_CSDebugEditorEdMode");

FCSDebugEditorEdMode::FCSDebugEditorEdMode()
{

}

FCSDebugEditorEdMode::~FCSDebugEditorEdMode()
{

}

void FCSDebugEditorEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FCSDebugEditorEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FCSDebugEditorEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

bool FCSDebugEditorEdMode::UsesToolkits() const
{
	return true;
}




