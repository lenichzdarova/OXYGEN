// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LyraInventoryItemDefinition.h"
#include "InventoryFragment_SpawnDefinition.generated.h"

/**
 * 
 */
UCLASS()
class OXYGEN_API UInventoryFragment_SpawnDefinition : public ULyraInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	// Интерактивный актор, который спавнится и из дропа, и из скрипта.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	TObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FVector MeshOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FVector MeshScale = FVector(1.f);
};
