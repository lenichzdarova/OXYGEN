// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActorBase.h"

AInteractableActorBase::AInteractableActorBase()
{
	
}

void AInteractableActorBase::GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder)
{	
	FInteractionOption Option;
	Option.Text = InteractionText;
	Option.InteractionWidgetClass = InteractionWidgetClass;
	Option.InteractionAbilityToGrant = InteractionAbilityToGrant;
	OptionBuilder.AddInteractionOption(Option);
}
