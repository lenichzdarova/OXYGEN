// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CollisionProfile.h"
#include "LyraGameplayAbility_Interact.h"
#include "OxygenGameplayAbility_PlayerInteract.generated.h"

class UAbilityTask_WaitForInteractableTargets_SingleLineTrace;
struct FInteractionOption;

/**
 * Player interaction ability.
 *
 * Lyra's base class only sphere-scans nearby interactables and grants their abilities.
 * This subclass adds the forward line trace Lyra normally does in GA_Interact, and feeds
 * the current option into UpdateInteractions so the matching interaction widget is shown.
 */
UCLASS()
class OXYGEN_API UOxygenGameplayAbility_PlayerInteract : public ULyraGameplayAbility_Interact
{
	GENERATED_BODY()

public:

	UOxygenGameplayAbility_PlayerInteract(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:

	void StartInteractionTrace();

	UFUNCTION()
	void HandleInteractableObjectsChanged(const TArray<FInteractionOption>& InteractiveOptions);

	/** How far in front of the avatar the look-at trace reaches. Kept shorter than the nearby grant scan. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionTraceRange = 200.f;

	/** Collision profile that overlaps Lyra_TraceChannel_Interaction. Lyra ships this as Interactable_BlockDynamic. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FCollisionProfileName InteractionTraceProfile = FCollisionProfileName(TEXT("Interactable_BlockDynamic"));

	/** Draws the look-at trace. Green is a miss, red is a blocking hit. */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	bool bShowInteractionTrace = true;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitForInteractableTargets_SingleLineTrace> InteractionTraceTask;
};
