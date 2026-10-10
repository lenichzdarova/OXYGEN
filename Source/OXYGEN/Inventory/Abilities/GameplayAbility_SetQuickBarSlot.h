// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/LyraGameplayAbility.h"
#include "GameplayAbility_SetQuickBarSlot.generated.h"

/**
 * 
 */
UCLASS()
class OXYGEN_API UGameplayAbility_SetQuickBarSlot : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGameplayAbility_SetQuickBarSlot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Возвращает индекс слота (0-4) по InputTag, или INDEX_NONE если тег не распознан */
	int32 GetSlotIndexFromInputTags(const FGameplayTagContainer& InputTags) const;
};
