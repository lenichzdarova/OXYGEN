// Fill out your copyright notice in the Description page of Project Settings.


#include "HabitatConnectorComponent.h"
#include "BuildingBlockDefinition.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tag_Building_Mesh_Type_Connector, "Building.Mesh.Type.Connector", "Connector mesh variant")

UHabitatConnectorComponent::UHabitatConnectorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHabitatConnectorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    RestoreConnections();
    Super::EndPlay(EndPlayReason);
}

void UHabitatConnectorComponent::InitConnection()
{
    RestoreConnections();

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    UBuildingBlockStaticMeshComponent* FoundA = nullptr;
    UBuildingBlockStaticMeshComponent* FoundB = nullptr;

    if (!TraceForComponent(Owner->GetActorForwardVector(), FoundA))
    {
        return;
    }
    if (!TraceForComponent(-Owner->GetActorForwardVector(), FoundB))
    {
        return;
    }

    CompA = FoundA;
    CompB = FoundB;
    ModuleA = FoundA->GetOwner();
    ModuleB = FoundB->GetOwner();

    if (CompA.IsValid())
    {
        CompA->SetVariant(Tag_Building_Mesh_Type_Connector);
    }
    if (CompB.IsValid())
    {
        CompB->SetVariant(Tag_Building_Mesh_Type_Connector);
    }

    if (ModuleA.IsValid())
    {
        ModuleA->OnDestroyed.AddDynamic(this, &UHabitatConnectorComponent::OnModuleDestroyed);
    }
    if (ModuleB.IsValid())
    {
        ModuleB->OnDestroyed.AddDynamic(this, &UHabitatConnectorComponent::OnModuleDestroyed);
    }
}

void UHabitatConnectorComponent::RestoreConnections()
{
    if (CompA.IsValid())
    {
        CompA->SetVariant(Tag_Building_Mesh_Type_Default);
    }
    if (CompB.IsValid())
    {
        CompB->SetVariant(Tag_Building_Mesh_Type_Default);
    }

    if (ModuleA.IsValid())
    {
        ModuleA->OnDestroyed.RemoveDynamic(this, &UHabitatConnectorComponent::OnModuleDestroyed);
    }
    if (ModuleB.IsValid())
    {
        ModuleB->OnDestroyed.RemoveDynamic(this, &UHabitatConnectorComponent::OnModuleDestroyed);
    }

    CompA = nullptr;
    CompB = nullptr;
    ModuleA = nullptr;
    ModuleB = nullptr;
}

bool UHabitatConnectorComponent::IsConnected() const
{
    return ModuleA.IsValid() && ModuleB.IsValid();
}

bool UHabitatConnectorComponent::TraceForComponent(const FVector& Direction, UBuildingBlockStaticMeshComponent*& OutComp) const
{
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return false;
    }

    const FVector Start = Owner->GetActorLocation();
    const FVector End = Start + Direction * TraceDistance;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);

    if (!Owner->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        return false;
    }

    UBuildingBlockStaticMeshComponent* Comp = Cast<UBuildingBlockStaticMeshComponent>(Hit.GetComponent());
    if (!Comp || !Comp->HasVariant(Tag_Building_Mesh_Type_Connector))
    {
        return false;
    }

    OutComp = Comp;
    return true;
}

void UHabitatConnectorComponent::OnModuleDestroyed(AActor* DestroyedActor)
{
    if (AActor* Owner = GetOwner())
    {
        Owner->Destroy();
    }
}
