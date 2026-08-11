// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "GameplayTagContainer.h"
#include "GameUIManagerSubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "O2_UISubsystem.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;
class UO2_PrimaryGameLayout;

/**
 * 
 */
UCLASS()
class OXYGEN_API UO2_UISubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()
	
virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
