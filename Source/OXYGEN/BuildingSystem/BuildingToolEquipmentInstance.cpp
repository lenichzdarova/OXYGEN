
#include "BuildingToolEquipmentInstance.h"
#include "BuildingSystem/OxygenBuildingBlueprintDefinition.h"
#include "Net/UnrealNetwork.h"

UBuildingToolEquipmentInstance::UBuildingToolEquipmentInstance()
{
}

void UBuildingToolEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UBuildingToolEquipmentInstance, PreviewActor);
    DOREPLIFETIME(UBuildingToolEquipmentInstance, SelectedBlueprint);
}

void UBuildingToolEquipmentInstance::SetSelectedBlueprint(UOxygenBuildingBlueprintDefinition* InBlueprint)
{    
    if (!GetPawn()->HasAuthority())
    {
        return;
    }
       

   // ClearSelectedBlueprint();

    SelectedBlueprint = InBlueprint;

    if (!SelectedBlueprint || !SelectedBlueprint->PreviewActor)
    {
        return;
    }
        

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }        

    FActorSpawnParameters Params;
    Params.Owner = GetPawn();
    Params.Instigator = GetPawn();
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    PreviewActor = World->SpawnActor<AActor>(SelectedBlueprint->PreviewActor, FTransform::Identity, Params);
    if (PreviewActor)
    {
        //PreviewActor->SetReplicates(true);
        //PreviewActor->SetReplicateMovement(true);
        BroadcastPreviewChanged();
    }    
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Preview Actor Spawned"));
}

void UBuildingToolEquipmentInstance::ClearSelectedBlueprint()
{
    if (!GetPawn()->HasAuthority())
        return;

    if (PreviewActor)
    {
        PreviewActor->Destroy();
        PreviewActor = nullptr;
        BroadcastPreviewChanged();
    }

    SelectedBlueprint = nullptr;
}

void UBuildingToolEquipmentInstance::OnUnequipped()
{
    ClearSelectedBlueprint();
    Super::OnUnequipped();
}

void UBuildingToolEquipmentInstance::OnRep_PreviewActor()
{
    BroadcastPreviewChanged();
}

void UBuildingToolEquipmentInstance::BroadcastPreviewChanged()
{
    OnPreviewChanged.Broadcast(PreviewActor);
}
