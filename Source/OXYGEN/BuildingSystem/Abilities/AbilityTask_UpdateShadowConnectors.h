// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "BuildingSystem/BuildingBlockStaticMeshComponent.h"
#include "AbilityTask_UpdateShadowConnectors.generated.h"


UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Blueprint_Connector_Short);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Blueprint_Connector_Long);
/**
 * 
 */
class ABuildingPreviewActor;
class UOxygenBuildingBlueprintDefinition;
class UBuildingBlueprintRegistry;

UCLASS()
class OXYGEN_API UAbilityTask_UpdateShadowConnectors : public UAbilityTask
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility"))
    static UAbilityTask_UpdateShadowConnectors* UpdateShadowConnectors(UGameplayAbility* OwningAbility, ABuildingPreviewActor* InPreview, float InInterval = 0.1f);

protected:
    virtual void Activate() override;
    virtual void OnDestroy(bool bInOwnerFinished) override;

    void TickUpdate();
    void ClearShadowConnectors();

    bool TryFindConnectorTarget(UBuildingBlockStaticMeshComponent* FromComp, UBuildingBlockStaticMeshComponent*& OutTarget, float& OutDistance) const;

    UOxygenBuildingBlueprintDefinition* SelectConnectorDefinition(float Distance) const;

    void SpawnShadowConnector(UBuildingBlockStaticMeshComponent* From, UBuildingBlockStaticMeshComponent* To, UOxygenBuildingBlueprintDefinition* Definition);

    UPROPERTY()
    TObjectPtr<ABuildingPreviewActor> Preview;

    UPROPERTY()
    TArray<TObjectPtr<AActor>> ShadowConnectors;

    float Interval = 0.1f;
    float MaxTraceDistance = 200.f;
    float AxisTolerance = 10.f;
    
    UPROPERTY(EditDefaultsOnly, Category="Connectors")
    float ShortConnectorLength = 100.f;

    UPROPERTY(EditDefaultsOnly, Category="Connectors")
    float LongConnectorLength = 200.f;

    UPROPERTY(EditDefaultsOnly, Category="Connectors")
    float LengthTolerance = 20.f;

    FTimerHandle TimerHandle;
};

