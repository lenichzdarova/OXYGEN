// Fill out your copyright notice in the Description page of Project Settings.


#include "OxygenActivatableWidget.h"

#include "CommonUITypes.h"
#include "LyraInputConfig.h"
#include "LyraPawnData.h"
#include "LyraPawnExtensionComponent.h"
#include "Input/CommonUIInputTypes.h"

void UOxygenActivatableWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (!CommonUI::IsEnhancedInputSupportEnabled() || !ExtraBackAction)
	{
		return;
	}

	FBindUIActionArgs BindArgs(ExtraBackAction, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction));
	BindArgs.bDisplayInActionBar = false;
	ExtraBackHandle = RegisterUIActionBinding(BindArgs);
}

void UOxygenActivatableWidget::NativeOnDeactivated()
{
	if (ExtraBackHandle.IsValid())
	{
		ExtraBackHandle.Unregister();
	}

	Super::NativeOnDeactivated();
}

void UOxygenActivatableWidget::HandleExtraBackAction()
{
	//DeactivateWidget();
}