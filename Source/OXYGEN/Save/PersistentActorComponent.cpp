// Fill out your copyright notice in the Description page of Project Settings.


#include "PersistentActorComponent.h"
#include "ActorRegistrySubsystem.h"
#include "Engine/World.h"

UPersistentActorComponent::UPersistentActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UPersistentActorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!PersistentId.IsValid())
	{
		PersistentId = FGuid::NewGuid();
	}

	RegisterWithRegistry();
}

void UPersistentActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterFromRegistry();
	Super::EndPlay(EndPlayReason);
}

void UPersistentActorComponent::SetPersistentId(const FGuid& InId)
{
	PersistentId = InId;
}

void UPersistentActorComponent::RegisterWithRegistry()
{
	if (UWorld* World = GetWorld())
	{
		if (UActorRegistrySubsystem* Registry = World->GetSubsystem<UActorRegistrySubsystem>())
		{
			Registry->RegisterActor(GetOwner(), PersistentId, bIsLevelPlaced);
		}
	}
}

void UPersistentActorComponent::UnregisterFromRegistry()
{
	if (UWorld* World = GetWorld())
	{
		if (UActorRegistrySubsystem* Registry = World->GetSubsystem<UActorRegistrySubsystem>())
		{
			Registry->UnregisterActor(PersistentId);
		}
	}
}
