// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Construction.h"

#include "BuildingSystem/BaseManagerSubsystem.h"
#include "BuildingSystem/HabitatConstructionComponent.h"
#include "BuildingSystem/BuildingPreviewActor.h"
#include "BuildingSystem/BuildingToolEquipmentInstance.h"
#include "BuildingSystem/OxygenBuildingBlueprintDefinition.h"
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

    ABuildingPreviewActor* Preview = GetPreviewActor();
    Server_StartConstruction(Preview);
}

void UGA_Construction::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    StopProgress();
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Construction::Server_StartConstruction_Implementation(AActor* InPreviewActor)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // 1. Если перед игроком уже есть строящийся объект — продолжаем его
    if (UHabitatConstructionComponent* Existing = FindConstructionTarget())
    {
        if (Existing->IsUnderConstruction())
        {
            ConstructionComp = Existing;
            TargetActor = Existing->GetOwner();

            World->GetTimerManager().SetTimer(
                ProgressTimer, this, &UGA_Construction::TickConstruction, 0.05f, true);
            return;
        }
    }

    // 2. Иначе ставим новый из превью
    if (!InPreviewActor)
    {
        return;
    }

    UBuildingToolEquipmentInstance* Equip = Cast<UBuildingToolEquipmentInstance>(GetAssociatedEquipment());
    if (!Equip || !Equip->SelectedBlueprint || !Equip->SelectedBlueprint->FinalActor)
    {
        return;
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    TargetActor = World->SpawnActor<AActor>(
        Equip->SelectedBlueprint->FinalActor,
        InPreviewActor->GetActorTransform(),
        Params);

    if (!TargetActor)
    {
        return;
    }
    
    if (UBaseManagerSubsystem* BaseSys = World->GetSubsystem<UBaseManagerSubsystem>())
    {
        const FBox Bounds = TargetActor->GetComponentsBoundingBox(true);
        BaseSys->RegisterPiece(TargetActor, Bounds);
    }

    if (ABuildingPreviewActor* Preview = Cast<ABuildingPreviewActor>(InPreviewActor))
    {
        Preview->CommitConnectors(TargetActor);
    }

    ConstructionComp = TargetActor->FindComponentByClass<UHabitatConstructionComponent>();
    if (ConstructionComp)
    {
        ConstructionComp->StartConstruction();
    }

    InPreviewActor->Destroy();

    World->GetTimerManager().SetTimer(
        ProgressTimer, this, &UGA_Construction::TickConstruction, 0.05f, true);
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