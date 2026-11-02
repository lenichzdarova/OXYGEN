// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PersistenceSubsystem.generated.h"


USTRUCT()
struct FSavedComponentData
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<UActorComponent> ComponentClass;

	UPROPERTY()
	TArray<uint8> SerializedData;
};

USTRUCT()
struct FSavedActorData
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid PersistentId;

	UPROPERTY()
	TSubclassOf<AActor> ActorClass;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	bool bIsLevelPlaced = false;

	UPROPERTY()
	TArray<uint8> ActorSerializedData;

	UPROPERTY()
	TArray<FSavedComponentData> Components;
};
/**
 * 
 */
UCLASS()
class OXYGEN_API UPersistenceSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Persistence")
	void SaveAllActors(TArray<FSavedActorData>& OutSavedActors);

	UFUNCTION(BlueprintCallable, Category = "Persistence")
	void LoadAllActors(const TArray<FSavedActorData>& SavedActors);

private:
	void StoreActor(AActor* Actor, FSavedActorData& OutData);
	void RestoreActor(AActor* Actor, const FSavedActorData& Data);
	AActor* SpawnAndRestore(const FSavedActorData& Data);
};
