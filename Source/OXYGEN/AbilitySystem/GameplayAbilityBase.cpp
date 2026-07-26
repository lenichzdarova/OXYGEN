// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilityBase.h"

#include "Engine/Engine.h"

bool UGameplayAbilityBase::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                              const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Yellow, TEXT("Checking ability activation"));
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}
