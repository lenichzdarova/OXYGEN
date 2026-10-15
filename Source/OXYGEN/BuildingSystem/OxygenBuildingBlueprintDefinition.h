// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OxygenBuildingBlueprintDefinition.generated.h"

/**
 * 
 */
UCLASS()
class OXYGEN_API UOxygenBuildingBlueprintDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	TSubclassOf<AActor> PreviewActor;	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	TSubclassOf<AActor> FinalActor;
	
};
