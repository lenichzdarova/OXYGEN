// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Engine/DataAsset.h"
#include "BuildingBlockDefinition.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Building_Mesh_Type_Default)

USTRUCT(BlueprintType)
struct FBuildingBlockVariant
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Meta = (Categories = "Building.Mesh.Type"))
	FGameplayTag VariantTag;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> Mesh;
};
/**
 * 
 */
UCLASS()
class OXYGEN_API UBuildingBlockDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Meta = (Categories = "Building.Block.Type"))
	FGameplayTag BlockType;

	// все возможные состояния этого куска
	UPROPERTY(EditAnywhere)
	TArray<FBuildingBlockVariant> Variants;

	UFUNCTION(BlueprintCallable)
	FBuildingBlockVariant GetVariantByTag(FGameplayTag VariantTag) const;
	UFUNCTION(BlueprintCallable)
	bool HaveVariant(FGameplayTag VariantTag) const;
};
