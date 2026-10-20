// Fill out your copyright notice in the Description page of Project Settings.


#include "HabitatConstructionComponent.h"
#include "Net/UnrealNetwork.h"

UHabitatConstructionComponent::UHabitatConstructionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UHabitatConstructionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UHabitatConstructionComponent, ConstructionProgress);
    DOREPLIFETIME(UHabitatConstructionComponent, bIsUnderConstruction);
    DOREPLIFETIME(UHabitatConstructionComponent, bIsCompleted);
}

void UHabitatConstructionComponent::StartConstruction()
{
    if (bIsUnderConstruction || bIsCompleted)
    {
        return;
    }

    bIsUnderConstruction = true;
    bIsCompleted = false;
    ConstructionProgress = 0.f;

    OnConstructionStarted.Broadcast();
    OnProgressChanged.Broadcast(ConstructionProgress);
    OnRep_State();
}

void UHabitatConstructionComponent::AddConstructionProgress(float Amount)
{
    if (!bIsUnderConstruction || bIsCompleted || Amount <= 0.f)
    {
        return;
    }

    if (!ConsumeResources(Amount))
    {
        return;
    }

    ConstructionProgress = FMath::Clamp(ConstructionProgress + Amount, 0.f, 1.f);
    OnRep_ConstructionProgress();

    if (bAutoCompleteAtFull && ConstructionProgress >= 1.f)
    {
        CompleteConstruction();
    }
}

void UHabitatConstructionComponent::CompleteConstruction()
{
    if (bIsCompleted)
    {
        return;
    }

    ConstructionProgress = 1.f;
    bIsUnderConstruction = false;
    bIsCompleted = true;

    OnProgressChanged.Broadcast(ConstructionProgress);
    OnConstructionCompleted.Broadcast();
    OnRep_State();
}

void UHabitatConstructionComponent::CancelConstruction()
{
    if (!bIsUnderConstruction)
    {
        return;
    }

    RefundResources(ConstructionProgress);

    bIsUnderConstruction = false;
    ConstructionProgress = 0.f;

    OnConstructionCancelled.Broadcast();
    OnRep_State();
}

void UHabitatConstructionComponent::StartDeconstruction()
{
    if (!bIsCompleted)
    {
        return;
    }

    bIsCompleted = false;
    bIsUnderConstruction = true;

    OnDeconstructionStarted.Broadcast();
    OnRep_State();
}

void UHabitatConstructionComponent::CompleteDeconstruction()
{
    RefundResources(1.f);

    bIsUnderConstruction = false;
    bIsCompleted = false;
    ConstructionProgress = 0.f;

    OnDeconstructionCompleted.Broadcast();
    OnRep_State();
}

void UHabitatConstructionComponent::OnRep_ConstructionProgress()
{
    OnProgressChanged.Broadcast(ConstructionProgress);
}

void UHabitatConstructionComponent::OnRep_State()
{
    // состояние реплицировано, при необходимости можно добавить отдельный делегат
}

bool UHabitatConstructionComponent::ConsumeResources(float ProgressDelta)
{
    // TODO: проверка и списание ресурсов
    return true;
}

void UHabitatConstructionComponent::RefundResources(float ProgressAmount)
{
    // TODO: возврат ресурсов
}