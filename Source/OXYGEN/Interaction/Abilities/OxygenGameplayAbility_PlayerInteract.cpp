// Copyright Epic Games, Inc. All Rights Reserved.

#include "OxygenGameplayAbility_PlayerInteract.h"

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Interaction/InteractionQuery.h"
#include "Interaction/Tasks/AbilityTask_WaitForInteractableTargets_SingleLineTrace.h"
#include "LyraLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OxygenGameplayAbility_PlayerInteract)

UOxygenGameplayAbility_PlayerInteract::UOxygenGameplayAbility_PlayerInteract(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOxygenGameplayAbility_PlayerInteract::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Presentation trace: only the owning client (or a listen-server host) has a view to aim from.
	if (IsLocallyControlled())
	{
		StartInteractionTrace();
	}
}

void UOxygenGameplayAbility_PlayerInteract::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsLocallyControlled())
	{
		UpdateInteractions(TArray<FInteractionOption>());
	}

	InteractionTraceTask = nullptr;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOxygenGameplayAbility_PlayerInteract::StartInteractionTrace()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	FInteractionQuery InteractionQuery;
	InteractionQuery.RequestingAvatar = AvatarActor;
	InteractionQuery.RequestingController = GetControllerFromActorInfo();

	// Trace starts at the avatar. AimWithPlayerController then bends the end point through the camera view,
	// which is the same setup GA_Interact uses for its single-line task.
	FGameplayAbilityTargetingLocationInfo StartLocation;
	StartLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
	StartLocation.SourceActor = AvatarActor;
	StartLocation.SourceAbility = this;

	InteractionTraceTask = UAbilityTask_WaitForInteractableTargets_SingleLineTrace::WaitForInteractableTargets_SingleLineTrace(
		this,
		InteractionQuery,
		InteractionTraceProfile,
		StartLocation,
		InteractionTraceRange,
		InteractionScanRate,
		bShowInteractionTrace);

	InteractionTraceTask->InteractableObjectsChanged.AddDynamic(this, &ThisClass::HandleInteractableObjectsChanged);
	InteractionTraceTask->ReadyForActivation();
}

void UOxygenGameplayAbility_PlayerInteract::HandleInteractableObjectsChanged(const TArray<FInteractionOption>& InteractiveOptions)
{
	UE_LOG(LogLyra, Verbose, TEXT("%s interaction trace options changed: %d"), *GetNameSafe(GetAvatarActorFromActorInfo()), InteractiveOptions.Num());

	// Picks InteractionWidgetClass from the current option, or DefaultInteractionWidgetClass, and shows it via the indicator manager.
	UpdateInteractions(InteractiveOptions);
}
