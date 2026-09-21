// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LyraInventoryItemDefinition.h"
#include "Equipment/LyraEquipmentDefinition.h"
#include "InventoryFragment_LoadoutItem.generated.h"

/**
 * 
 */
UCLASS()
class OXYGEN_API UInventoryFragment_LoadoutItem : public ULyraInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	//допустимые слоты
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	FGameplayTag SlotTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	TSubclassOf<ULyraEquipmentDefinition> EquipmentDefinition;
};
