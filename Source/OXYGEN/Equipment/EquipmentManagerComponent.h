// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentDefinition.h"
#include "EquipmentInstance.h"
#include "Components/ActorComponent.h"
#include "EquipmentManagerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OXYGEN_API UEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEquipmentManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	
	
	UFUNCTION()
	UEquipmentInstance* EquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition);
	
	UFUNCTION()
	void UnequipItem(UEquipmentInstance* EquipmentInstance);
};
