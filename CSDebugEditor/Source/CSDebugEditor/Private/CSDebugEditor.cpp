// Copyright Epic Games, Inc. All Rights Reserved.

#include "CSDebugEditor.h"
#include "CSDebugEditorEdMode.h"

#define LOCTEXT_NAMESPACE "FCSDebugEditorModule"

void FCSDebugEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FCSDebugEditorEdMode>(FCSDebugEditorEdMode::EM_CSDebugEditorEdModeId, LOCTEXT("CSDebugEditorEdModeName", "CSDebugEditorEdMode"), FSlateIcon(), true);
}

void FCSDebugEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FCSDebugEditorEdMode::EM_CSDebugEditorEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCSDebugEditorModule, CSDebugEditor)