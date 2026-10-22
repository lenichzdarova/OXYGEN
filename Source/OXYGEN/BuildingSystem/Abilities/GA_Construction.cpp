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

            World->GetTimerManager().SetTimer(ProgressTimer, this, &UGA_Construction::TickConstruction, 0.05f, true);
            return;
        }
    }

    UBuildingToolEquipmentInstance* Equip = Cast<UBuildingToolEquipmentInstance>(GetAssociatedEquipment());
    if (!Equip)
    {
        return;
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 2. Спавним все финальные объекты из PendingPieces (модуль + коннекторы)
    for (const FPendingBuildPiece& Piece : Equip->PendingPieces)
    {
        if (!Piece.PreviewActor || !Piece.Definition || !Piece.Definition->FinalActor)
        {
            continue;
        }

        AActor* Spawned = World->SpawnActor<AActor>(Piece.Definition->FinalActor, Piece.PreviewActor->GetActorTransform(), Params);
        if (!Spawned)
        {
            continue;
        }

        if (UBaseManagerSubsystem* BaseSys = World->GetSubsystem<UBaseManagerSubsystem>())
        {
            const FBox Bounds = Spawned->GetComponentsBoundingBox(true);
            BaseSys->RegisterPiece(Spawned, Bounds);
        }

        if (UHabitatConstructionComponent* Comp = Spawned->FindComponentByClass<UHabitatConstructionComponent>())
        {
            Comp->StartConstruction();

            // запоминаем первый как основной для тика прогресса
            if (!ConstructionComp)
            {
                ConstructionComp = Comp;
                TargetActor = Spawned;
            }
        }

        Piece.PreviewActor->Destroy();
    }

    Equip->PendingPieces.Empty();

    if (ConstructionComp)
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