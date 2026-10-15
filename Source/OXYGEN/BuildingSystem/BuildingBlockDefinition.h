// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Engine/DataAsset.h"
#include "BuildingBlockDefinition.generated.h"


UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Building_Block_Type_Wall)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Building_Block_Type_Floor)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Building_Block_Type_Ceiling)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Building_Block_Type_Connector)

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Building_Mesh_Type_Default)
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
	
	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh;
};
