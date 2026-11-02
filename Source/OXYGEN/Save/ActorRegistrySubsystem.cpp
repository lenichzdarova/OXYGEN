// ActorRegistrySubsystem.cpp
#include "ActorRegistrySubsystem.h"

void UActorRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UActorRegistrySubsystem::Deinitialize()
{
	RegisteredActors.Empty();
	LevelPlacedFlags.Empty();
	Super::Deinitialize();
}

void UActorRegistrySubsystem::RegisterActor(AActor* Actor, const FGuid& PersistentId, bool bIsLevelPlaced)
{
	if (!Actor || !PersistentId.IsValid())
	{
		return;
	}

	RegisteredActors.Add(PersistentId, Actor);
	LevelPlacedFlags.Add(PersistentId, bIsLevelPlaced);

	OnActorRegistered.Broadcast(Actor, PersistentId);
}

void UActorRegistrySubsystem::UnregisterActor(const FGuid& PersistentId)
{
	RegisteredActors.Remove(PersistentId);
	LevelPlacedFlags.Remove(PersistentId);

	OnActorUnregistered.Broadcast(PersistentId);
}

AActor* UActorRegistrySubsystem::FindActorById(const FGuid& PersistentId) const
{
	if (const TWeakObjectPtr<AActor>* Found = RegisteredActors.Find(PersistentId))
	{
		return Found->Get();
	}
	return nullptr;
}

TArray<AActor*> UActorRegistrySubsystem::GetAllRegisteredActors() const
{
	TArray<AActor*> Result;
	for (const auto& Pair : RegisteredActors)
	{
		if (AActor* Actor = Pair.Value.Get())
		{
			Result.Add(Actor);
		}
	}
	return Result;
}