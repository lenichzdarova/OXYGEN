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
}

void UGA_Construction::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!IsLocallyControlled()) return; 
    Server_StartConstruction();
}

void UGA_Construction::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    StopProgress();
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_Construction::IsInputStillHeld() const
{    
    return GetCurrentAbilitySpec()->InputPressed;
}

void UGA_Construction::Server_StartConstruction_Implementation()
{    
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    UHabitatConstructionComponent* Existing = FindConstructionTarget();
    if (!Existing)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    if (!Existing->IsUnderConstruction() && !Existing->IsCompleted())
    {
        Existing->StartConstruction();
    }

    if (!Existing->IsUnderConstruction())
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    ConstructionComp = Existing;
    TargetActor = Existing->GetOwner();

    World->GetTimerManager().SetTimer(ProgressTimer, this, &UGA_Construction::TickConstruction, 0.05f, true);
}

void UGA_Construction::TickConstruction()
{
    if (!ConstructionComp || !IsActive())
    {
        StopProgress();
        return;
    }

    if (!IsInputStillHeld())
    {
        StopProgress();
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar || !TargetActor)
    {
        StopProgress();
        return;
    }

    const float Dist = FVector::Dist(Avatar->GetActorLocation(), TargetActor->GetActorLocation());
    if (Dist > TraceDistance)
    {
        StopProgress();
        return;
    }

    ConstructionComp->AddConstructionProgress(ProgressPerSecond * 0.05f);

    if (ConstructionComp->IsCompleted())
    {
        StopProgress();
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
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
    if (!PC || !PC->PlayerCameraManager) return nullptr;

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