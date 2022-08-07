// Copyright 2020 SensyuGames.
/**
 * @file CSDebug.h
 * @brief CSDebug‚Ìmodule
 * @author SensyuGames
 * @date 2020/5/25
 */

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class CSDEBUG_API FCSDebugModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
