// Copyright 2020 SensyuGames.
/**
 * @file CSDebugEditor.cpp
 * @brief CSDebugEditor
 * @author SensyuGames
 * @date 2020/7/24
 */

#include "CSDebugEditor.h"
#include "DataTableRowSelectorCustomization.h"

#define LOCTEXT_NAMESPACE "FCSDebugEditorModule"

void FCSDebugEditorModule::StartupModule()
{
	auto& moduleMgr = FModuleManager::Get();
	if (moduleMgr.IsModuleLoaded("PropertyEditor")) {

		auto& propertyEditorModule = moduleMgr.LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		propertyEditorModule.RegisterCustomPropertyTypeLayout(
			("DataTableRowSelector"),
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDataTableRowSelectorCustomization::MakeInstance)
		);

		propertyEditorModule.NotifyCustomizationModuleChanged();
	}
}

void FCSDebugEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCSDebugEditorModule, CSDebugEditor)