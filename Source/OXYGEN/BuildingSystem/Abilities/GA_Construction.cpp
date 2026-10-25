// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Construction.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "BuildingSystem/HabitatConstructionComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

UGA_Construction::UGA_Construction()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;    
    ActivationPolicy = ELyraAbilityActivationPolicy::WhileInputActive;
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UGA_Construction::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!IsLocallyControlled())
    {
        return;
    }
    
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Construction in progress..."));
    Server_StartConstruction();
}

void UGA_Construction::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    StopProgress();
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Construction::Server_StartConstruction_Implementation()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    World->GetTimerManager().SetTimer(ProgressTimer, this, &UGA_Construction::TickConstruction, TimerRate, true);
}

void UGA_Construction::TickConstruction()
{       
    const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
    if (!Spec || !Spec->InputPressed)
    {       
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    UHabitatConstructionComponent* Current = FindConstructionTarget();
    if (!Current)
    {
        return;
    }
    float Amount = TimerRate * Current->GetConstructionRate();
    Current->AddConstructionProgress(Amount);    
}

void UGA_Construction::StopProgress()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(ProgressTimer);
    }
}

UHabitatConstructionComponent* UGA_Construction::FindConstructionTarget() const
{
    APlayerController* PC = GetActorInfo().PlayerController.Get();
    if (!PC || !PC->PlayerCameraManager)
    {
        return nullptr;
    }

    const FVector Start = PC->PlayerCameraManager->GetCameraLocation();
    const FVector End = Start + PC->PlayerCameraManager->GetCameraRotation().Vector() * TraceDistance;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetAvatarActorFromActorInfo());

    if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        return nullptr;
    }

    if (AActor* HitActor = Hit.GetActor())
    {
        return HitActor->FindComponentByClass<UHabitatConstructionComponent>();
    }

    return nullptr;
}