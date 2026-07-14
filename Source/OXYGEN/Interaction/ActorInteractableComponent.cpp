// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorInteractableComponent.h"


// Sets default values for this component's properties
UActorInteractableComponent::UActorInteractableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UActorInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UActorInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UActorInteractableComponent::Interact(AActor* Instigator, AController* Controller)
{	
	if (bInteractionEnabled)
	{
		OnInteractionStarted.Broadcast(Instigator, Controller);
	}	
}

void UActorInteractableComponent::StopInteract(AActor* Instigator, AController* Controller)
{
	OnInteractionEnded.Broadcast(Instigator, Controller);
}

void UActorInteractableComponent::Highlight(bool bEnable)
{
}

