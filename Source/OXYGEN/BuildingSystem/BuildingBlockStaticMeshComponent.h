// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBlockDefinition.h"
#include "Components/StaticMeshComponent.h"
#include "BuildingBlockStaticMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class OXYGEN_API UBuildingBlockStaticMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	
	UBuildingBlockStaticMeshComponent();
	
	UPROPERTY()
	FGameplayTag CurrentMeshType = Tag_Building_Mesh_Type_Default;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBuildingBlockDefinition> BlockDefinition;
	
	UFUNCTION(BlueprintCallable)
	void SetBlockDefinition(UBuildingBlockDefinition* InBlockDefinition);
	
protected:
	void OnDefinitionSet();
};
