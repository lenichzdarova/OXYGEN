#include "InteractExecutionAbility.h"
#include "ActorInteractableComponent.h"
#include "AbilitySystemComponent.h"

UInteractExecutionAbility::UInteractExecutionAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	// По умолчанию серверная, но можно настроить
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	// Настройка триггера на GameplayEvent
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;	
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Interact.Execute"));
	AbilityTriggers.Add(TriggerData);	
}

void UInteractExecutionAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	
	if (!TriggerEventData)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AActor* Target = const_cast<AActor*>(TriggerEventData->Target.Get());
	if (!Target)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	PerformInteract(Target);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);	
}

void UInteractExecutionAbility::PerformInteract(AActor* Target)
{	
	UActorInteractableComponent* InteractComp = Target->GetComponentByClass<UActorInteractableComponent>();
	if (!InteractComp) return;

	if (AActor* Instigator = GetAvatarActorFromActorInfo())
	{
		InteractComp->Interact(Instigator, Instigator->GetInstigatorController());		
	}	
}