// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

OXYGEN_API DECLARE_LOG_CATEGORY_EXTERN(LogLyra, Log, All);
OXYGEN_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraExperience, Log, All);
OXYGEN_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraAbilitySystem, Log, All);
OXYGEN_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraTeams, Log, All);

OXYGEN_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
