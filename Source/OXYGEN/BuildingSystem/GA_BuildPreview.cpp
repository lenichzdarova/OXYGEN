// GA_BuildPreview.cpp
#include "GA_BuildPreview.h"
#include "BuildingToolEquipmentInstance.h"
#include "BaseManagerSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_BuildPreview::UGA_BuildPreview()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    ActivationPolicy = ELyraAbilityActivationPolicy::OnSpawn;
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UGA_BuildPreview::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (UBuildingToolEquipmentInstance* Equip = GetBuildingEquipment())
    {        
        Equip->OnPreviewChanged.AddDynamic(this, &UGA_BuildPreview::OnPreviewChanged);        

        if (Equip->PreviewActor)
        {
            HandlePreviewAdded(Equip->PreviewActor);
        }
    }
}

void UGA_BuildPreview::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (UBuildingToolEquipmentInstance* Equip = GetBuildingEquipment())
    {
        Equip->OnPreviewChanged.RemoveDynamic(this, &UGA_BuildPreview::OnPreviewChanged);
    }

    StopPreviewUpdate();
    PreviewActor = nullptr;

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_BuildPreview::OnPreviewChanged(AActor* InPreview)
{
    
    if (InPreview)
    {
        HandlePreviewAdded(InPreview);       
    }
    else
    {
        HandlePreviewRemoved(InPreview);        
    }
}

void UGA_BuildPreview::HandlePreviewAdded(AActor* InPreview)
{
    PreviewActor = InPreview;
    StartPreviewUpdate();
}

void UGA_BuildPreview::HandlePreviewRemoved(AActor* InPreview)
{
    StopPreviewUpdate();
    PreviewActor = nullptr;
}

UBuildingToolEquipmentInstance* UGA_BuildPreview::GetBuildingEquipment() const
{
    return Cast<UBuildingToolEquipmentInstance>(GetAssociatedEquipment());
}

void UGA_BuildPreview::StartPreviewUpdate()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(UpdateTimer, this, &UGA_BuildPreview::UpdatePreview, UpdateInterval, true);
    }
}

void UGA_BuildPreview::StopPreviewUpdate()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(UpdateTimer);
    }
}

void UGA_BuildPreview::UpdatePreview()
{
    if (!IsLocallyControlled() || !PreviewActor)
    {
        return;
    }
        
    Server_UpdatePreview(GetLookLocation(), PreviewActor->GetActorRotation());    
}

FVector UGA_BuildPreview::GetLookLocation() const
{
    if (APlayerController* PC = GetActorInfo().PlayerController.Get())
    {
        if (PC->PlayerCameraManager)
        {
            return PC->PlayerCameraManager->GetCameraLocation() +
                   PC->PlayerCameraManager->GetCameraRotation().Vector() * PreviewDistance;
        }
    }    
    
    return FVector::ZeroVector;
}

void UGA_BuildPreview::Server_UpdatePreview_Implementation(FVector_NetQuantize DesiredLocation, FRotator DesiredRotation)
{
    
    if (!PreviewActor)
    {
        return;
    }     

    if (UBaseManagerSubsystem* BaseSys = GetWorld()->GetSubsystem<UBaseManagerSubsystem>())
    {
        FTransform Desired(DesiredRotation, DesiredLocation);
        FTransform Final = BaseSys->GetPreviewTransform(Desired);
        PreviewActor->SetActorTransform(Final);        
    }    
}
