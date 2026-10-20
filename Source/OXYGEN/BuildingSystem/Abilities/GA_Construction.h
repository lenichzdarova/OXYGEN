// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/LyraGameplayAbility_FromEquipment.h"
#include "GA_Construction.generated.h"

class UHabitatConstructionComponent;
class ABuildingPreviewActor;

UCLASS()
class OXYGEN_API UGA_Construction : public ULyraGameplayAbility_FromEquipment
{
	GENERATED_BODY()

public:
	UGA_Construction();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Construction")
	float ProgressPerSecond = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category="Construction")
	float TraceDistance = 500.f;

	UPROPERTY()
	TObjectPtr<AActor> TargetActor;

	UPROPERTY()
	TObjectPtr<UHabitatConstructionComponent> ConstructionComp;

	FTimerHandle ProgressTimer;

	void TickConstruction();
	void StopProgress();

	UFUNCTION(Server, Reliable)
	void Server_StartConstruction(AActor* InPreviewActor);

	ABuildingPreviewActor* GetPreviewActor() const;
	UHabitatConstructionComponent* FindConstructionTarget() const;
};
