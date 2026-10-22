// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingSystem/BuildingToolEquipmentInstance.h"
#include "Equipment/LyraGameplayAbility_FromEquipment.h"
#include "GA_BuildPreview.generated.h"

class UAbilityTask_UpdateShadowConnectors;

UCLASS()
class OXYGEN_API UGA_BuildPreview : public ULyraGameplayAbility_FromEquipment
{
	GENERATED_BODY()

public:
	UGA_BuildPreview();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Preview")
	float PreviewDistance = 500.f;

	UPROPERTY(EditDefaultsOnly, Category="Preview")
	float UpdateInterval = 0.05f;

	UPROPERTY()
	TObjectPtr<AActor> PreviewActor;

	UPROPERTY()
	TObjectPtr<UAbilityTask_UpdateShadowConnectors> ShadowConnectorTask;

	FTimerHandle UpdateTimer;

	UFUNCTION()
	void OnPreviewChanged(AActor* InPreview);

	UFUNCTION()
	void HandlePreviewAdded(AActor* InPreview);

	UFUNCTION()
	void HandlePreviewRemoved(AActor* InPreview);

	void StartPreviewUpdate();
	void StopPreviewUpdate();
	void UpdatePreview();

	void StartShadowConnectorTask();
	void StopShadowConnectorTask();

	UFUNCTION(Server, Reliable)
	void Server_UpdatePreview(FVector_NetQuantize DesiredLocation, FRotator DesiredRotation);

	FVector GetLookLocation() const;

	UBuildingToolEquipmentInstance* GetBuildingEquipment() const;
};

