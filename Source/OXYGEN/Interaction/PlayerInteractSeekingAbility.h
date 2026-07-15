#pragma once

#include "CoreMinimal.h"
#include "OXYGEN/AbilitySystem/GameplayAbilityBase.h"
#include "InteractExecutionAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "AbilityTask_WaitForInteractableTarget.h"
#include "PlayerInteractSeekingAbility.generated.h"

UCLASS()
class OXYGEN_API UPlayerInteractSeekingAbility : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UPlayerInteractSeekingAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UFUNCTION()
	void OnTargetFound(AActor* InteractableActor);

	UFUNCTION()
	void OnTargetLost(AActor* InteractableActor);

	void ActivateExecutionAbility(AActor* Target);

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitForInteractableTarget> TraceTask;

	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentInteractableTarget;

	// Класс исполнительной способности, которую будем активировать
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FGameplayTag DefaultAbilityTag = FGameplayTag::RequestGameplayTag("Ability.Interact");
	
};