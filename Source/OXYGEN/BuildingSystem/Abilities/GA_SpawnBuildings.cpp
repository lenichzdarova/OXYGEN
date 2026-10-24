// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SpawnBuildings.h"

#include "BuildingSystem/BaseManagerSubsystem.h"
#include "BuildingSystem/HabitatConstructionComponent.h"
#include "BuildingSystem/BuildingToolEquipmentInstance.h"
#include "BuildingSystem/OxygenBuildingBlueprintDefinition.h"

UGA_SpawnBuildings::UGA_SpawnBuildings()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    ActivationPolicy = ELyraAbilityActivationPolicy::OnInputTriggered;
}

void UGA_SpawnBuildings::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (IsLocallyControlled())
    {
        Server_SpawnBuilding();
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_SpawnBuildings::Server_SpawnBuilding_Implementation()
{
    UWorld* World = GetWorld();
    if (!World) return;

    UBuildingToolEquipmentInstance* Equip = Cast<UBuildingToolEquipmentInstance>(GetAssociatedEquipment());
    if (!Equip) return;

    TArray<FPendingBuildPiece> PiecesToSpawn = Equip->PendingPieces;
    Equip->PendingPieces.Empty();

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (const FPendingBuildPiece& Piece : PiecesToSpawn)
    {
        if (!Piece.PreviewActor || !Piece.Definition || !Piece.Definition->FinalActor)
        {
            continue;
        }

        AActor* Spawned = World->SpawnActor<AActor>(Piece.Definition->FinalActor, Piece.PreviewActor->GetActorTransform(), Params);
        if (!Spawned) continue;

        if (UBaseManagerSubsystem* BaseSys = World->GetSubsystem<UBaseManagerSubsystem>())
        {
            const FBox Bounds = Spawned->GetComponentsBoundingBox(true);
            BaseSys->RegisterPiece(Spawned, Bounds);
        }

        Piece.PreviewActor->Destroy();
    }
}