// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GhostConnectorBase.generated.h"

UCLASS(Abstract, Blueprintable)
class OXYGEN_API AGhostConnectorBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGhostConnectorBase();

	/** Класс постоянного коннектора, который будет заспавнен при коммите. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Building|Connector")
	TSubclassOf<AActor> PersistentClass;

	/** Заспавнить постоянный коннектор и уничтожить призрак. */
	UFUNCTION(BlueprintCallable, Category="Building|Connector")
	virtual void Commit();

protected:
	virtual void BeginPlay() override;
};
