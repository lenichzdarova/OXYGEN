// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/LyraGameplayAbility_FromEquipment.h"
#include "GA_Deconstruction.generated.h"

class UHabitatConstructionComponent;
/**
 * 
 */
UCLASS()
class OXYGEN_API UGA_Deconstruction : public ULyraGameplayAbility_FromEquipment
{
	GENERATED_BODY()
	
	UGA_Deconstruction();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Deconstruction")
	float ProgressPerSecond = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category="Deconstruction")
	float TraceDistance = 500.f;

	UPROPERTY()
	TObjectPtr<UHabitatConstructionComponent> ConstructionComp;

	FTimerHandle ProgressTimer;

	void TickDeconstruction();
	void StopProgress();

	UFUNCTION(Server, Reliable)
	void Server_StartDeconstruction();

	UHabitatConstructionComponent* FindConstructionTarget() const;
};
