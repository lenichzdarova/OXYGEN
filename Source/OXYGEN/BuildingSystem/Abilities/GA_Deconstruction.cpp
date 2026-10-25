// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Deconstruction.h"
#include "BuildingSystem/HabitatConstructionComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

UGA_Deconstruction::UGA_Deconstruction()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    ActivationPolicy = ELyraAbilityActivationPolicy::WhileInputActive;
}

void UGA_Deconstruction::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    Server_StartDeconstruction();
}

void UGA_Deconstruction::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    StopProgress();
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Deconstruction::Server_StartDeconstruction_Implementation()
{
    ConstructionComp = FindConstructionTarget();
    if (!ConstructionComp)
    {
        return;
    }

   // ConstructionComp->StartDeconstruction();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(ProgressTimer, this, &UGA_Deconstruction::TickDeconstruction, 0.05f, true);
    }
}

void UGA_Deconstruction::TickDeconstruction()
{
    if (!ConstructionComp)
    {
        StopProgress();
        return;
    }

    // уменьшаем прогресс; когда дойдёт до 0 — CompleteDeconstruction
    const float NewProgress = ConstructionComp->GetProgress() - ProgressPerSecond * 0.05f;
    if (NewProgress <= 0.f)
    {
        //ConstructionComp->CompleteDeconstruction();
        StopProgress();
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    // если хочешь уменьшать через Add с отрицательным — добавь поддержку в компонент
    // пока просто вызываем Complete при достижении нуля
}

void UGA_Deconstruction::StopProgress()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(ProgressTimer);
    }
}

UHabitatConstructionComponent* UGA_Deconstruction::FindConstructionTarget() const
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