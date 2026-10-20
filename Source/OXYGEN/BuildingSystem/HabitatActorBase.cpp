// Fill out your copyright notice in the Description page of Project Settings.


#include "HabitatActorBase.h"

AHabitatActorBase::AHabitatActorBase()
{
	bReplicates = true;
	//SetReplicateMovement(true);
	PrimaryActorTick.bCanEverTick = false;

	ConstructionComponent = CreateDefaultSubobject<UHabitatConstructionComponent>(TEXT("ConstructionComponent"));
}

void AHabitatActorBase::BeginPlay()
{
	Super::BeginPlay();

	if (ConstructionComponent)
	{
		ConstructionComponent->OnConstructionStarted.AddDynamic(this, &AHabitatActorBase::HandleConstructionStarted);
		ConstructionComponent->OnProgressChanged.AddDynamic(this, &AHabitatActorBase::HandleProgressChanged);
		ConstructionComponent->OnConstructionCompleted.AddDynamic(this, &AHabitatActorBase::HandleConstructionCompleted);
		ConstructionComponent->OnConstructionCancelled.AddDynamic(this, &AHabitatActorBase::HandleConstructionCancelled);
		ConstructionComponent->OnDeconstructionStarted.AddDynamic(this, &AHabitatActorBase::HandleDeconstructionStarted);
		ConstructionComponent->OnDeconstructionCompleted.AddDynamic(this, &AHabitatActorBase::HandleDeconstructionCompleted);
	}
}

void AHabitatActorBase::HandleConstructionStarted_Implementation()
{
}

void AHabitatActorBase::HandleProgressChanged_Implementation(float NewProgress)
{
}

void AHabitatActorBase::HandleConstructionCompleted_Implementation()
{
}

void AHabitatActorBase::HandleConstructionCancelled_Implementation()
{
}

void AHabitatActorBase::HandleDeconstructionStarted_Implementation()
{
}

void AHabitatActorBase::HandleDeconstructionCompleted_Implementation()
{
}

