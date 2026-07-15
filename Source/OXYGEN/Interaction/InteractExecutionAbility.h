#pragma once

#include "CoreMinimal.h"
#include "OXYGEN/AbilitySystem/GameplayAbilityBase.h"
#include "InteractExecutionAbility.generated.h"

UCLASS()
class OXYGEN_API UInteractExecutionAbility : public UGameplayAbilityBase
{
    GENERATED_BODY()

public:
    UInteractExecutionAbility();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

protected:
    void PerformInteract(AActor* Target);
};