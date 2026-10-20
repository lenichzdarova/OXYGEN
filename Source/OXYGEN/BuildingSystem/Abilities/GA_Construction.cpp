// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Construction.h"

#include "BuildingSystem/HabitatConstructionComponent.h"
#include "BuildingSystem/BuildingPreviewActor.h"
#include "BuildingSystem/BuildingToolEquipmentInstance.h"

UGA_Construction::UGA_Construction()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    ActivationPolicy = ELyraAbilityActivationPolicy::WhileInputActive;
}

void UGA_Construction::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (ABuildingPreviewActor* Preview = GetPreviewActor())
    {
        Server_StartConstruction(Preview);
    }
}

void UGA_Construction::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    StopProgress();
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Construction::Server_StartConstruction_Implementation(AActor* PreviewActor)
{
    if (!PreviewActor)
    {
        return;
    }

    // TODO: заспавнить постоянный актор на месте превью + коннекторы
    // TargetActor = SpawnedActor;
    // ConstructionComp = TargetActor->FindComponentByClass<UConstructionComponent>();

    if (ConstructionComp)
    {
        ConstructionComp->StartConstruction();
    }

    PreviewActor->Destroy();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(ProgressTimer, this, &UGA_Construction::TickConstruction, 0.05f, true);
    }
}

void UGA_Construction::TickConstruction()
{
    if (!ConstructionComp)
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

ABuildingPreviewActor* UGA_Construction::GetPreviewActor() const
{
    if (UBuildingToolEquipmentInstance* Equip = Cast<UBuildingToolEquipmentInstance>(GetAssociatedEquipment()))
    {
        return Cast<ABuildingPreviewActor>(Equip->PreviewActor);
    }
    return nullptr;
}