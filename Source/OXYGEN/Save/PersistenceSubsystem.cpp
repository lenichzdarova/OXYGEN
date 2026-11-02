// Fill out your copyright notice in the Description page of Project Settings.


#include "PersistenceSubsystem.h"
#include "ActorRegistrySubsystem.h"
#include "PersistentActorComponent.h"
#include "PersistentObjectInterface.h"
#include "Serializer.h"
#include "Engine/World.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

void UPersistenceSubsystem::SaveAllActors(TArray<FSavedActorData>& OutSavedActors)
{
	OutSavedActors.Empty();

	UActorRegistrySubsystem* Registry = GetWorld()->GetSubsystem<UActorRegistrySubsystem>();
	if (!Registry) return;

	for (AActor* Actor : Registry->GetAllRegisteredActors())
	{
		if (!Actor) continue;

		FSavedActorData Data;
		StoreActor(Actor, Data);
		OutSavedActors.Add(Data);
	}
}

void UPersistenceSubsystem::LoadAllActors(const TArray<FSavedActorData>& SavedActors)
{
	UWorld* World = GetWorld();
	if (!World) return;

	UActorRegistrySubsystem* Registry = World->GetSubsystem<UActorRegistrySubsystem>();
	if (!Registry) return;

	TSet<FGuid> ProcessedIds;

	for (const FSavedActorData& Data : SavedActors)
	{
		AActor* Actor = Registry->FindActorById(Data.PersistentId);

		if (!Actor && !Data.bIsLevelPlaced)
		{
			Actor = SpawnAndRestore(Data);
		}
		else if (Actor)
		{
			RestoreActor(Actor, Data);
		}

		if (Actor)
		{
			if (Actor->Implements<UPersistentObjectInterface>())
			{
				IPersistentObjectInterface::Execute_OnAfterLoad(Actor);
			}
			ProcessedIds.Add(Data.PersistentId);
		}
	}

	// Удаляем level-placed, которых нет в сейве
	for (AActor* Actor : Registry->GetAllRegisteredActors())
	{
		if (!Actor) continue;

		if (UPersistentActorComponent* Comp = Actor->FindComponentByClass<UPersistentActorComponent>())
		{
			if (Comp->IsLevelPlaced() && !ProcessedIds.Contains(Comp->GetPersistentId()))
			{
				Actor->Destroy();
			}
		}
	}
}

void UPersistenceSubsystem::StoreActor(AActor* Actor, FSavedActorData& OutData)
{
	UPersistentActorComponent* Comp = Actor->FindComponentByClass<UPersistentActorComponent>();
	if (!Comp) return;

	OutData.PersistentId = Comp->GetPersistentId();
	OutData.bIsLevelPlaced = Comp->IsLevelPlaced();
	OutData.ActorClass = Actor->GetClass();
	OutData.Transform = Actor->GetActorTransform();

	Serializer::SerializeObject(Actor, OutData.ActorSerializedData);

	for (UActorComponent* Component : Actor->GetComponents())
	{
		if (!Component) continue;

		FSavedComponentData CompData;
		CompData.ComponentClass = Component->GetClass();
		Serializer::SerializeObject(Component, CompData.SerializedData);
		OutData.Components.Add(CompData);
	}
}

void UPersistenceSubsystem::RestoreActor(AActor* Actor, const FSavedActorData& Data)
{
	if (!Actor || Data.ActorClass != Actor->GetClass()) return;

	Actor->SetActorTransform(Data.Transform);
	Serializer::DeserializeObject(Actor, Data.ActorSerializedData);

	for (const FSavedComponentData& CompData : Data.Components)
	{
		UActorComponent* Component = Actor->GetComponentByClass(CompData.ComponentClass);
		if (!Component)
		{
			Component = Actor->AddComponentByClass(CompData.ComponentClass, false, FTransform::Identity, true);
		}

		if (Component)
		{
			Serializer::DeserializeObject(Component, CompData.SerializedData);
		}
	}
}

AActor* UPersistenceSubsystem::SpawnAndRestore(const FSavedActorData& Data)
{
	if (!Data.ActorClass) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.bDeferConstruction = true;

	AActor* Actor = GetWorld()->SpawnActor<AActor>(Data.ActorClass, Data.Transform, Params);
	if (!Actor) return nullptr;

	if (UPersistentActorComponent* Comp = Actor->FindComponentByClass<UPersistentActorComponent>())
	{
		Comp->SetPersistentId(Data.PersistentId);
	}

	Actor->FinishSpawning(Data.Transform);
	RestoreActor(Actor, Data);

	return Actor;
}