// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_UpdateShadowConnectors.h"

#include "BuildingSystem/BuildingPreviewActor.h"
#include "BuildingSystem/BuildingBlueprintRegistry.h"
#include "BuildingSystem/OxygenBuildingBlueprintDefinition.h"
#include "BuildingSystem/BuildingBlockDefinition.h"
#include "BuildingSystem/BuildingToolEquipmentInstance.h"
#include "Engine/GameInstance.h"
#include "Equipment/LyraGameplayAbility_FromEquipment.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tag_Blueprint_Connector_Short, "Blueprint.Connector.Short", "Blueprint Connector Short");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tag_Blueprint_Connector_Long, "Blueprint.Connector.Long", "Blueprint Connector Long");

UAbilityTask_UpdateShadowConnectors* UAbilityTask_UpdateShadowConnectors::UpdateShadowConnectors(UGameplayAbility* OwningAbility, ABuildingPreviewActor* InPreview, float InInterval)
{
    UAbilityTask_UpdateShadowConnectors* Task = NewAbilityTask<UAbilityTask_UpdateShadowConnectors>(OwningAbility);
    Task->Preview = InPreview;
    Task->Interval = InInterval;
    return Task;
}

void UAbilityTask_UpdateShadowConnectors::Activate()
{
    Super::Activate();

    if (!Preview)
    {
        EndTask();
        return;
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(TimerHandle, this, &UAbilityTask_UpdateShadowConnectors::TickUpdate, Interval, true);
    }
}

void UAbilityTask_UpdateShadowConnectors::OnDestroy(bool bInOwnerFinished)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TimerHandle);
    }

    ClearShadowConnectors();
    Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_UpdateShadowConnectors::TickUpdate()
{
    if (!Preview)
    {
        return;
    }

    ClearShadowConnectors();

    TArray<UBuildingBlockStaticMeshComponent*> AllComps;
    Preview->GetComponents<UBuildingBlockStaticMeshComponent>(AllComps);

    for (UBuildingBlockStaticMeshComponent* Comp : AllComps)
    {
        if (!Comp || !Comp->HasVariant(Tag_Building_Mesh_Type_Connector))
        {
            continue;
        }

        UBuildingBlockStaticMeshComponent* Target = nullptr;
        float Distance = 0.f;

        if (!TryFindConnectorTarget(Comp, Target, Distance))
        {
            continue;
        }

        UOxygenBuildingBlueprintDefinition* Definition = SelectConnectorDefinition(Distance);
        if (!Definition || !Definition->PreviewActor)
        {
            continue;
        }

        SpawnShadowConnector(Comp, Target, Definition);
    }
}

void UAbilityTask_UpdateShadowConnectors::ClearShadowConnectors()
{
    UBuildingToolEquipmentInstance* Equip = nullptr;

    if (ULyraGameplayAbility_FromEquipment* EquipAbility = Cast<ULyraGameplayAbility_FromEquipment>(Ability))
    {
        Equip = Cast<UBuildingToolEquipmentInstance>(EquipAbility->GetAssociatedEquipment());
    }

    for (AActor* Conn : ShadowConnectors)
    {
        if (Equip)
        {
            Equip->PendingPieces.RemoveAll([Conn](const FPendingBuildPiece& Piece)
            {
                return Piece.PreviewActor == Conn;
            });
        }

        if (Conn)
        {
            Conn->Destroy();
        }
    }

    ShadowConnectors.Empty();
}

bool UAbilityTask_UpdateShadowConnectors::TryFindConnectorTarget(UBuildingBlockStaticMeshComponent* FromComp, UBuildingBlockStaticMeshComponent*& OutTarget, float& OutDistance) const
{
    if (!FromComp || !Preview)
    {
        return false;
    }

    const FVector Start = FromComp->GetComponentLocation();
    const FVector End = Start + FromComp->GetForwardVector() * MaxTraceDistance;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Preview);

    for (AActor* Shadow : ShadowConnectors)
    {
        Params.AddIgnoredActor(Shadow);
    }

    if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        return false;
    }

    UBuildingBlockStaticMeshComponent* Other = Cast<UBuildingBlockStaticMeshComponent>(Hit.GetComponent());
    if (!Other || !Other->HasVariant(Tag_Building_Mesh_Type_Connector))
    {
        return false;
    }

    const FVector LocA = FromComp->GetComponentLocation();
    const FVector LocB = Other->GetComponentLocation();
    const FVector Delta = LocB - LocA;

    OutDistance = Delta.Size();

    if (OutDistance <= 80.f)
    {
        return false;
    }

    const FVector Dir = Delta.GetSafeNormal();
    if (Dir.IsNearlyZero())
    {
        return false;
    }

    const float Dot = FMath::Abs(FVector::DotProduct(FromComp->GetForwardVector(), Dir));
    if (Dot < 0.95f)
    {
        return false;
    }

    const float HeightDiff = FMath::Abs(FVector::DotProduct(Delta, FromComp->GetUpVector()));
    if (HeightDiff > AxisTolerance)
    {
        return false;
    }

    OutTarget = Other;
    return true;
}

UOxygenBuildingBlueprintDefinition* UAbilityTask_UpdateShadowConnectors::SelectConnectorDefinition(float Distance) const
{
    UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    if (!GI)
    {
        return nullptr;
    }

    UBuildingBlueprintRegistry* Registry = GI->GetSubsystem<UBuildingBlueprintRegistry>();
    if (!Registry)
    {
        return nullptr;
    }

    FGameplayTag SelectedTag;

    if (FMath::Abs(Distance - ShortConnectorLength) <= LengthTolerance)
    {
        SelectedTag = Tag_Blueprint_Connector_Short;
    }
    else if (FMath::Abs(Distance - LongConnectorLength) <= LengthTolerance)
    {
        SelectedTag = Tag_Blueprint_Connector_Long;
    }
    else
    {
        return nullptr;
    }

    return Registry->GetDefinition(SelectedTag);
}

void UAbilityTask_UpdateShadowConnectors::SpawnShadowConnector(UBuildingBlockStaticMeshComponent* From, UBuildingBlockStaticMeshComponent* To, UOxygenBuildingBlueprintDefinition* Definition)
{
    if (!From || !To || !Definition || !Definition->PreviewActor || !Preview)
    {
        return;
    }

    const FVector Mid = (From->GetComponentLocation() + To->GetComponentLocation()) * 0.5f;
    const FRotator Rot = (To->GetComponentLocation() - From->GetComponentLocation()).Rotation();

    FActorSpawnParameters Params;
    Params.Owner = Preview;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* Shadow = GetWorld()->SpawnActor<AActor>(Definition->PreviewActor, Mid, Rot, Params);
    if (!Shadow)
    {
        return;
    }

    ShadowConnectors.Add(Shadow);

    if (ULyraGameplayAbility_FromEquipment* EquipAbility = Cast<ULyraGameplayAbility_FromEquipment>(Ability))
    {
        if (UBuildingToolEquipmentInstance* Equip = Cast<UBuildingToolEquipmentInstance>(EquipAbility->GetAssociatedEquipment()))
        {
            FPendingBuildPiece Piece;
            Piece.PreviewActor = Shadow;
            Piece.Definition = Definition;
            Equip->PendingPieces.Add(Piece);
        }
    }
}