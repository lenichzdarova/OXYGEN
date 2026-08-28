#pragma once

#include "CoreMinimal.h"
#include "OXYGEN/AbilitySystem/GameGameplayAbilityBase.h"
#include "InteractExecutionAbility.generated.h"

UCLASS()
class OXYGEN_API UInteractExecutionAbility : public UGameGameplayAbilityBase
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