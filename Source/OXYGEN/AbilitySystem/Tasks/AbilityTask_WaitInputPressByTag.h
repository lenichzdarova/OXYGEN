// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayTagContainer.h"
#include "AbilityTask_WaitInputPressByTag.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInputPressedByTag);

UCLASS()
class OXYGEN_API UAbilityTask_WaitInputPressByTag : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnInputPressedByTag OnPress;

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UAbilityTask_WaitInputPressByTag* WaitInputPressByTag(UGameplayAbility* OwningAbility, FGameplayTag InputTag);

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

protected:
	UFUNCTION()
	void OnInputPressed();

	FGameplayTag WatchedTag;
};
