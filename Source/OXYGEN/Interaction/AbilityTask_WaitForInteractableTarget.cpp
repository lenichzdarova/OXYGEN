#include "AbilityTask_WaitForInteractableTarget.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "Interactable.h" // предполагаем, что ваш интерфейс здесь

UAbilityTask_WaitForInteractableTarget::UAbilityTask_WaitForInteractableTarget()
{
    bTickingTask = false; // используем таймер, не тик
}

UAbilityTask_WaitForInteractableTarget* UAbilityTask_WaitForInteractableTarget::WaitForInteractableTarget(
    UGameplayAbility* OwningAbility,
    float TraceDistance,
    ECollisionChannel TraceChannel,
    float UpdateInterval)
{
    UAbilityTask_WaitForInteractableTarget* Task = NewAbilityTask<UAbilityTask_WaitForInteractableTarget>(OwningAbility);
    Task->TraceDistance = TraceDistance;
    Task->TraceChannel = TraceChannel;
    Task->UpdateInterval = UpdateInterval;
    return Task;
}

void UAbilityTask_WaitForInteractableTarget::Activate()
{
    Super::Activate();
    // Первый трасс сразу
    PerformTrace();
    // Запускаем таймер для периодического обновления
    GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this, &UAbilityTask_WaitForInteractableTarget::PerformTrace, UpdateInterval, true);    
}

void UAbilityTask_WaitForInteractableTarget::OnDestroy(bool bInOwnerFinished)
{
    GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
    Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitForInteractableTarget::PerformTrace()
{    
    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* Controller = World->GetFirstPlayerController();    
    if (!Controller) return;

    APlayerCameraManager* CameraManager = Controller->PlayerCameraManager;
    if (!CameraManager) return;

    FVector Start = CameraManager->GetCameraLocation();
    FVector End = Start + CameraManager->GetCameraRotation().Vector() * TraceDistance;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Ability->GetAvatarActorFromActorInfo());

    AActor* NewTarget = nullptr;
    if (World->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params))
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && HitActor->Implements<UInteractable>())
        {
            NewTarget = HitActor;
        }
    }
    UpdateTarget(NewTarget);   
    if (CurrentTarget.IsValid())
    {
        FString TargetName = CurrentTarget->GetName();
        GEngine->AddOnScreenDebugMessage(111, 5.f, FColor::Red, TargetName);
    }  
}

void UAbilityTask_WaitForInteractableTarget::UpdateTarget(AActor* NewTarget)
{
    if (CurrentTarget == NewTarget)
    {       
        return;
    }

    // Target changed
    if (CurrentTarget.IsValid())
    {
        OnTargetLost.Broadcast(CurrentTarget.Get());
    }
    
    CurrentTarget = NewTarget;
    if (NewTarget)
    {
        OnTargetFound.Broadcast(NewTarget);
    }
    OnTargetUpdated.Broadcast(NewTarget);    
}