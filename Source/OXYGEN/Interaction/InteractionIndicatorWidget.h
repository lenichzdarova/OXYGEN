// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "IndicatorSystem/IActorIndicatorWidget.h"

#include "InteractionIndicatorWidget.generated.h"


class UImage;
class UCommonTextBlock;
class ULyraActionWidget;
class ULyraInputConfig;
class UTexture2D;

USTRUCT(BlueprintType)
struct FOxygenInteractionPrompt
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FText Text;

	UPROPERTY(BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	// Пустой тег — глиф не показываем.
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag InputTag;
};
/**
 * 
 */
UCLASS(Abstract)
class OXYGEN_API UInteractionIndicatorWidget : public UUserWidget, public IIndicatorWidgetInterface
{
	GENERATED_BODY()
	
public:
	virtual void BindIndicator_Implementation(UIndicatorDescriptor* Indicator) override;
	virtual void UnbindIndicator_Implementation(const UIndicatorDescriptor* Indicator) override;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	bool BuildPrompt(UObject* DataObject, FOxygenInteractionPrompt& OutPrompt);
	virtual bool BuildPrompt_Implementation(UObject* DataObject, FOxygenInteractionPrompt& OutPrompt);

	void ApplyPrompt(const FOxygenInteractionPrompt& Prompt);

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TObjectPtr<ULyraInputConfig> InputConfig;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> ActionIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> ActionText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<ULyraActionWidget> ActionInput;
	
};
