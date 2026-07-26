#include "InteractExecutionAbility.h"
#include "ActorInteractableComponent.h"
#include "AbilitySystemComponent.h"
#include "Engine/Engine.h"

UInteractExecutionAbility::UInteractExecutionAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	// По умолчанию серверная, но можно настроить
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	// Настройка триггера на GameplayEvent
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;	
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(FName("GAS.Ability.Interact"));
	AbilityTriggers.Add(TriggerData);	
}

void UInteractExecutionAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthority(&ActivationInfo))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Yellow, TEXT("Interact ability activated"));
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
	}
	
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