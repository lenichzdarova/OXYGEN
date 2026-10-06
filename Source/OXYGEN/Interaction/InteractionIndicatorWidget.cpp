// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionIndicatorWidget.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Input/LyraInputConfig.h"
#include "UI/Foundation/LyraActionWidget.h"
#include "UI/IndicatorSystem/IndicatorDescriptor.h"

void UInteractionIndicatorWidget::BindIndicator_Implementation(UIndicatorDescriptor* Indicator)
{
	UObject* DataObject = Indicator ? Indicator->GetDataObject() : nullptr;

	FOxygenInteractionPrompt Prompt;
	if (BuildPrompt(DataObject, Prompt))
	{
		ApplyPrompt(Prompt);
	}
	else
	{
		UnbindIndicator_Implementation(Indicator);
	}
}

void UInteractionIndicatorWidget::UnbindIndicator_Implementation(const UIndicatorDescriptor* Indicator)
{
	if (ActionIcon)
	{
		ActionIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (ActionText)
	{
		ActionText->SetText(FText::GetEmpty());
	}
	if (ActionInput)
	{
		ActionInput->SetVisibility(ESlateVisibility::Collapsed);
		ActionInput->SetEnhancedInputAction(nullptr);
	}
}

bool UInteractionIndicatorWidget::BuildPrompt_Implementation(UObject* DataObject, FOxygenInteractionPrompt& OutPrompt)
{
	return false;
}

void UInteractionIndicatorWidget::ApplyPrompt(const FOxygenInteractionPrompt& Prompt)
{
	if (ActionText)
	{
		ActionText->SetText(Prompt.Text);
		ActionText->SetVisibility(Prompt.Text.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
	}

	if (ActionIcon)
	{
		if (UTexture2D* Icon = Prompt.Icon.LoadSynchronous())
		{
			ActionIcon->SetBrushFromTexture(Icon);
			ActionIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			ActionIcon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (ActionInput)
	{
		const UInputAction* InputAction = nullptr;
		if (InputConfig && Prompt.InputTag.IsValid())
		{
			InputAction = InputConfig->FindAbilityInputActionForTag(Prompt.InputTag, false);
		}

		ActionInput->SetEnhancedInputAction(const_cast<UInputAction*>(InputAction));
		ActionInput->SetVisibility(InputAction ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}