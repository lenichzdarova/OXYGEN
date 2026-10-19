#include "BuildingPreviewActor.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tag_Building_Mesh_Type_Connector, "Building.Mesh.Type.Connector", "Connector mesh variant")

ABuildingPreviewActor::ABuildingPreviewActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABuildingPreviewActor::UpdateConnectorPreview()
{
    ClearPreviewConnectors();

    const TArray<UBuildingBlockStaticMeshComponent*> MyComps = GetConnectorComponents();

    for (UBuildingBlockStaticMeshComponent* Comp : MyComps)
    {
        UBuildingBlockStaticMeshComponent* Target = nullptr;
        float Distance = 0.f;

        if (!TryFindConnectorTarget(Comp, Target, Distance))
        {
            continue;
        }

        const FConnectorLengthOption* Option = FindBestLengthOption(Distance);
        if (!Option || !Option->ConnectorClass)
        {
            continue;
        }

        SpawnPreviewConnector(Comp, Target, *Option);
    }
}

void ABuildingPreviewActor::CommitConnectors()
{
    // Здесь можно пометить коннекторы как финальные, если нужно.
    // Пока просто очищаем список превью, не уничтожая акторы.
    PreviewConnectors.Empty();
}

void ABuildingPreviewActor::ClearPreviewConnectors()
{
    for (AActor* Conn : PreviewConnectors)
    {
        if (Conn)
        {
            Conn->Destroy();
        }
    }
    PreviewConnectors.Empty();
}

TArray<UBuildingBlockStaticMeshComponent*> ABuildingPreviewActor::GetConnectorComponents() const
{
    TArray<UBuildingBlockStaticMeshComponent*> Result;
    TArray<UBuildingBlockStaticMeshComponent*> All;
    GetComponents<UBuildingBlockStaticMeshComponent>(All);

    for (UBuildingBlockStaticMeshComponent* Comp : All)
    {
        if (Comp && Comp->HasVariant(Tag_Building_Mesh_Type_Connector))
        {
            Result.Add(Comp);
        }
    }
    return Result;
}

bool ABuildingPreviewActor::TryFindConnectorTarget( UBuildingBlockStaticMeshComponent* FromComp, UBuildingBlockStaticMeshComponent*& OutTarget, float& OutDistance) const
{
    if (!FromComp)
    {
        return false;
    }

    const FVector Start = FromComp->GetComponentLocation();
    const FVector End = Start + FromComp->GetForwardVector() * MaxTraceDistance;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

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

const FConnectorLengthOption* ABuildingPreviewActor::FindBestLengthOption(float Distance) const
{
    const FConnectorLengthOption* Best = nullptr;
    float BestDiff = FLT_MAX;

    for (const FConnectorLengthOption& Option : ConnectorOptions)
    {
        const float Diff = FMath::Abs(Distance - Option.Length);
        if (Diff <= Option.Tolerance && Diff < BestDiff)
        {
            BestDiff = Diff;
            Best = &Option;
        }
    }
    return Best;
}

void ABuildingPreviewActor::SpawnPreviewConnector(
    UBuildingBlockStaticMeshComponent* From,
    UBuildingBlockStaticMeshComponent* To,
    const FConnectorLengthOption& Option)
{
    if (!Option.ConnectorClass)
    {
        return;
    }

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    const FVector Mid = (From->GetComponentLocation() + To->GetComponentLocation()) * 0.5f;
    const FRotator Rot = (To->GetComponentLocation() - From->GetComponentLocation()).Rotation();

    AActor* Conn = GetWorld()->SpawnActor<AActor>(
        Option.ConnectorClass, Mid, Rot, Params);

    if (Conn)
    {
        PreviewConnectors.Add(Conn);
    }
}