// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PersistentActorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OXYGEN_API UPersistentActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPersistentActorComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Persistence")
	FGuid GetPersistentId() const { return PersistentId; }

	UFUNCTION(BlueprintCallable, Category = "Persistence")
	bool IsLevelPlaced() const { return bIsLevelPlaced; }

	/** Для dynamic-акторов: выставить Id до BeginPlay (через SpawnActorDeferred) */
	void SetPersistentId(const FGuid& InId);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persistence", SaveGame)
	FGuid PersistentId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persistence")
	bool bIsLevelPlaced = true;

private:
	void RegisterWithRegistry();
	void UnregisterFromRegistry();
};
};
