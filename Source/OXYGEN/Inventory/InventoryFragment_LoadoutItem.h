#pragma once

#include "GameplayTagContainer.h"
#include "LyraInventoryItemDefinition.h"
#include "Equipment/LyraEquipmentDefinition.h"
#include "InventoryFragment_LoadoutItem.generated.h"

UCLASS()
class OXYGEN_API UInventoryFragment_LoadoutItem : public ULyraInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	FGameplayTagContainer LoadoutTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	bool IsUnique = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	TSubclassOf<ULyraEquipmentDefinition> EquipmentDefinition;
};