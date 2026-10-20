// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostConnectorBase.h"

AGhostConnectorBase::AGhostConnectorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void AGhostConnectorBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGhostConnectorBase::Commit()
{
	if (!PersistentClass)
	{
		Destroy();
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		Destroy();
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = GetOwner();

	AActor* Persistent = World->SpawnActor<AActor>(
		PersistentClass,
		GetActorTransform(),
		Params);

	// Постоянный актор сам через UHabitatConnectorComponent
	// найдёт строящийся модуль и подпишется на него.

	Destroy();
}