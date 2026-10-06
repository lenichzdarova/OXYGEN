// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IInteractableTarget.h"
#include "GameFramework/Actor.h"
#include "InteractableActorBase.generated.h"

UCLASS(Blueprintable, BlueprintType)
class OXYGEN_API AInteractableActorBase : public AActor, public IInteractableTarget
{
	GENERATED_BODY()

public:
	AInteractableActorBase();

	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder) override;

protected:	

	UPROPERTY(EditAnywhere, Category = "Interaction")
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	FText InteractionText;
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	TSubclassOf<UGameplayAbility> InteractionAbilityToGrant;
};

