#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "ActorInteractableComponent.h"
#include "AbilityTask_WaitForInteractableTarget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableTargetDelegate, AActor*, InteractableActor);

UCLASS()
class OXYGEN_API UAbilityTask_WaitForInteractableTarget : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_WaitForInteractableTarget();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_WaitForInteractableTarget* WaitForInteractableTarget(
		UGameplayAbility* OwningAbility,
		float TraceDistance = 500.0f,
		ECollisionChannel TraceChannel = ECollisionChannel::ECC_Visibility,
		float UpdateInterval = 0.1f
	);

	// Called when a new target is found (different from previous frame)
	UPROPERTY(BlueprintAssignable)
	FInteractableTargetDelegate OnTargetFound;

	// Called when the current target is lost (no interactable actor under crosshair)
	UPROPERTY(BlueprintAssignable)
	FInteractableTargetDelegate OnTargetLost;

	// Called every update with the current target (could be null)
	UPROPERTY(BlueprintAssignable)
	FInteractableTargetDelegate OnTargetUpdated;

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	void PerformTrace();
	void UpdateTarget(AActor* NewTarget);

	FTimerHandle TraceTimerHandle;
	float TraceDistance;
	ECollisionChannel TraceChannel;
	float UpdateInterval;

	TWeakObjectPtr<AActor> CurrentTarget;
};