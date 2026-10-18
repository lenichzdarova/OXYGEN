// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "BuildingBlockDefinition.h"
#include "Components/StaticMeshComponent.h"
#include "BuildingBlockStaticMeshComponent.generated.h"

UCLASS(ClassGroup=(Building), meta=(BlueprintSpawnableComponent))
class OXYGEN_API UBuildingBlockStaticMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UBuildingBlockStaticMeshComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Building")
	TObjectPtr<UBuildingBlockDefinition> BlockDefinition;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentVariant, BlueprintReadOnly, Category="Building")
	FGameplayTag CurrentVariant;

	UFUNCTION(BlueprintCallable, Category="Building")
	void SetBlockDefinition(UBuildingBlockDefinition* InDefinition);

	UFUNCTION(BlueprintCallable, Category="Building")
	bool SetVariant(FGameplayTag NewVariant);

	UFUNCTION(BlueprintCallable, Category="Building")
	bool HasVariant(FGameplayTag VariantTag) const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION()
	void OnRep_CurrentVariant();

	void ApplyCurrentVariant();
};
