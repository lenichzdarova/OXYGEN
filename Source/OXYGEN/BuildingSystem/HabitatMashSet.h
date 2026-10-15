// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HabitatMashSet.generated.h"

USTRUCT(BlueprintType)
struct FBaseMeshEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Id;                    // "Wall", "Floor", "Ceiling", "WallCorner"

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh = nullptr;
};

UCLASS(BlueprintType)
class OXYGEN_API UHabitatMashSet : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FBaseMeshEntry> Meshes;
};
