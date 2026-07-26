#include "PlayerInteractSeekingAbility.h"

#include <string>

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"

UPlayerInteractSeekingAbility::UPlayerInteractSeekingAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;    
    NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UPlayerInteractSeekingAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData)
{     
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    // Запускаем трассировку
    TraceTask = UAbilityTask_WaitForInteractableTarget::WaitForInteractableTarget(
        this, 500.0f, ECC_Visibility, 0.1f);
    TraceTask->OnTargetFound.AddDynamic(this, &UPlayerInteractSeekingAbility::OnTargetFound);
    TraceTask->OnTargetLost.AddDynamic(this, &UPlayerInteractSeekingAbility::OnTargetLost);
    TraceTask->ReadyForActivation();        
    // Не завершаем способность, она должна висеть постоянно
}

void UPlayerInteractSeekingAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility, bool bWasCancelled)
{
    if (TraceTask) TraceTask->EndTask();   
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPlayerInteractSeekingAbility::InputPressed(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputPressed(Handle, ActorInfo, ActivationInfo);  
    
    AActor* Target = CurrentInteractableTarget.Get();
    if (Target)
    {
        ActivateExecutionAbility(Target);
    }
}

void UPlayerInteractSeekingAbility::OnTargetFound(AActor* InteractableActor)
{
    if (InteractableActor && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green,
            FString::Printf(TEXT(">> %s"), *InteractableActor->GetName()));
    }
    CurrentInteractableTarget = InteractableActor;
}

void UPlayerInteractSeekingAbility::OnTargetLost(AActor* InteractableActor)
{
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, TEXT("No target"));
    CurrentInteractableTarget = nullptr;
}

void UPlayerInteractSeekingAbility::ActivateExecutionAbility(AActor* Target)
{   
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();    
    if (!ASC) return;
    FGameplayTag AbilityTag = DefaultAbilityTag;    
    
    // Подготавливаем событие для передачи цели
    FGameplayEventData EventData;
    EventData.EventTag = AbilityTag;
    EventData.Instigator = GetAvatarActorFromActorInfo();
    EventData.Target = Target;
    EventData.OptionalObject = Target; // можно и так

    // Активируем способность через событие
    auto Result = ASC->HandleGameplayEvent(EventData.EventTag, &EventData);     
}