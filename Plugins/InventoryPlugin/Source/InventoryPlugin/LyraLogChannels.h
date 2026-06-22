// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

INVENTORYPLUGIN_API DECLARE_LOG_CATEGORY_EXTERN(LogLyra, Log, All);
INVENTORYPLUGIN_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraExperience, Log, All);
INVENTORYPLUGIN_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraAbilitySystem, Log, All);
INVENTORYPLUGIN_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraTeams, Log, All);

INVENTORYPLUGIN_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
