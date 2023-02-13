// Copyright 2020 SensyuGames.
/**
 * @file CSDebugEditor.h
 * @brief CSDebugEditor
 * @author SensyuGames
 * @date 2020/7/24
 */

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FCSDebugEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
