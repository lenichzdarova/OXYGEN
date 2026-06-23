// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OXYGEN/AbilitySystem/O2_GameplayAbilityBase.h"

#include "O2_GameplayAbility_FromEquipment.generated.h"

class UEquipmentInstance;
class UInventoryItemInstance;

/**
 * ULyraGameplayAbility_FromEquipment
 *
 * An ability granted by and associated with an equipment instance
 */
UCLASS()
class UO2_GameplayAbility_FromEquipment : public UO2_GameplayAbilityBase
{
	GENERATED_BODY()

public:

	UO2_GameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Lyra|Ability")
	UEquipmentInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	UInventoryItemInstance* GetAssociatedItem() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

};
