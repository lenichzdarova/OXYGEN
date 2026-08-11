// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonUIExtensions.h"
#include "GameUIPolicy.h"
#include "NativeGameplayTags.h"
#include "Engine/DeveloperSettings.h"
#include "O2_UISettings.generated.h"

class UO2_PrimaryGameLayout;

OXYGEN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layer_Game);
OXYGEN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layer_GameMenu);
OXYGEN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layer_Menu);
OXYGEN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layer_Modal);
/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Oxygen UI"))
class OXYGEN_API UO2_UISettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UO2_UISettings()
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("Oxygen UI");
	}	
	
	UPROPERTY(EditAnywhere, Config, Category = "UI")
	TSoftClassPtr<UGameUIPolicy> UIPolicyClass;
	
	UPROPERTY(EditAnywhere, Config, Category = "UI")
	TSoftClassPtr<UCommonActivatableWidget> MainMenuWidgetClass;
	
	virtual FName GetCategoryName() const override { return TEXT("Game"); }
};
