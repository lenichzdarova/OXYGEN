// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/LyraGameplayAbility_FromEquipment.h"
#include "GA_SpawnBuildings.generated.h"

/**
 * 
 */
UCLASS()
class OXYGEN_API UGA_SpawnBuildings : public ULyraGameplayAbility_FromEquipment
{
	GENERATED_BODY()
	
public:
	UGA_SpawnBuildings();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(Server, Reliable)
	void Server_SpawnBuilding();
};
