// Fill out your copyright notice in the Description page of Project Settings.


#include "HabitatConstructionComponent.h"
#include "Net/UnrealNetwork.h"

UHabitatConstructionComponent::UHabitatConstructionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

bool UHabitatConstructionComponent::IsUnderConstruction() const
{
    return ConstructionProgress < 1.f;
}

void UHabitatConstructionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UHabitatConstructionComponent, ConstructionProgress);
}

void UHabitatConstructionComponent::PerformConstruction()
{
    auto DeltaTime = GetWorld()->GetDeltaSeconds();
    float Progress = ProgressDeltaPerSecond * DeltaTime;
    AddConstructionProgress(Progress);
}

void UHabitatConstructionComponent::AddConstructionProgress(float Amount)
{
    if (!IsUnderConstruction())
    {
        OnConstructionCompleted.Broadcast();
        return;
    }
    
    ConstructionProgress = FMath::Clamp(ConstructionProgress + Amount, 0.f, 1.f);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Construction progress: ") + FString::SanitizeFloat(ConstructionProgress));
    OnRep_ConstructionProgress();
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