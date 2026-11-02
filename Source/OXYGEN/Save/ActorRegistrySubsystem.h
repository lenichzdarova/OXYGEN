// ActorRegistrySubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ActorRegistrySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorRegistered, AActor*, Actor, FGuid, PersistentId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorUnregistered, FGuid, PersistentId);

UCLASS()
class OXYGEN_API UActorRegistrySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void RegisterActor(AActor* Actor, const FGuid& PersistentId, bool bIsLevelPlaced);
	void UnregisterActor(const FGuid& PersistentId);

	UFUNCTION(BlueprintCallable, Category = "Registry")
	AActor* FindActorById(const FGuid& PersistentId) const;

	UFUNCTION(BlueprintCallable, Category = "Registry")
	TArray<AActor*> GetAllRegisteredActors() const;

	UPROPERTY(BlueprintAssignable, Category = "Registry")
	FOnActorRegistered OnActorRegistered;

	UPROPERTY(BlueprintAssignable, Category = "Registry")
	FOnActorUnregistered OnActorUnregistered;

private:
	TMap<FGuid, TWeakObjectPtr<AActor>> RegisteredActors;
	TMap<FGuid, bool> LevelPlacedFlags;
};