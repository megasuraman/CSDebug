// Copyright 2020 SensyuGames.
/**
 * @file CSDebug.cpp
 * @brief CSDebug��module
 * @author SensyuGames
 * @date 2020/5/25
 */

#include "CSDebug.h"
#include "CSDebugConfig.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#endif

#define LOCTEXT_NAMESPACE "FCSDebugModule"

void FCSDebugModule::StartupModule()
{
#if WITH_EDITOR
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		SettingsModule->RegisterSettings(
			"Project", //�v���W�F�N�g�ݒ�ɏo�����߂�
			"Plugins",
			"CSDebug",
			LOCTEXT("CSDebugName", "CSDebug"),
			LOCTEXT("CSDebugDescription", "CSDebug Config"),
			GetMutableDefault<UCSDebugConfig>()
		);
	}
#endif
}

void FCSDebugModule::ShutdownModule()
{
#if WITH_EDITOR
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings(
			"Project", //�v���W�F�N�g�ݒ�ɏo�����߂�
			"Plugins",
			"CSDebug"
		);
	}
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCSDebugModule, CSDebug)