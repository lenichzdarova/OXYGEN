// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LyraActivatableWidget.h"
#include "OxygenActivatableWidget.generated.h"

/**
 * 
 */
UCLASS()
class OXYGEN_API UOxygenActivatableWidget : public ULyraActivatableWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ExtraBackAction;

	void HandleExtraBackAction();

	FUIActionBindingHandle ExtraBackHandle;
};
