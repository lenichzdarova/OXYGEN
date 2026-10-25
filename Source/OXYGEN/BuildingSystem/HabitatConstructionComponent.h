// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HabitatConstructionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConstructionEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConstructionProgressChanged, float, NewProgress);

UCLASS(ClassGroup=(Building), meta=(BlueprintSpawnableComponent))
class OXYGEN_API UHabitatConstructionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHabitatConstructionComponent();

UPROPERTY(BlueprintAssignable, Category="Building")
    FOnConstructionEvent OnConstructionStarted;

    UPROPERTY(BlueprintAssignable, Category="Building")
    FOnConstructionEvent OnConstructionCompleted;

    UPROPERTY(BlueprintAssignable, Category="Building")
    FOnConstructionEvent OnConstructionCancelled;

    UPROPERTY(BlueprintAssignable, Category="Building")
    FOnConstructionEvent OnDeconstructionStarted;

    UPROPERTY(BlueprintAssignable, Category="Building")
    FOnConstructionEvent OnDeconstructionCompleted;

    UPROPERTY(BlueprintAssignable, Category="Building")
    FOnConstructionProgressChanged OnProgressChanged;

	void PerformConstruction();
	
    UFUNCTION(BlueprintCallable, Category="Building")
    void AddConstructionProgress(float Amount);
	
	UFUNCTION(BlueprintCallable, Category="Building")
	float GetConstructionRate() { return ProgressDeltaPerSecond; }

    UFUNCTION(BlueprintPure, Category="Building")
    float GetProgress() const { return ConstructionProgress; }

    UFUNCTION(BlueprintPure, Category="Building")
    bool IsUnderConstruction() const;    

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    UPROPERTY(ReplicatedUsing=OnRep_ConstructionProgress, BlueprintReadOnly, Category="Building")
    float ConstructionProgress = 0.f;   
	
	UPROPERTY()
	float ProgressDeltaPerSecond = 0.15f;

    UFUNCTION()
    void OnRep_ConstructionProgress();

    UFUNCTION()
    void OnRep_State();

    /** Потребление ресурсов на дельту прогресса. Пока заглушка. */
    virtual bool ConsumeResources(float ProgressDelta);

    /** Возврат ресурсов. Пока заглушка. */
    virtual void RefundResources(float ProgressAmount);
};
