// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingBlockDefinition.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tag_Building_Mesh_Type_Default, "Building.Mesh.Type.Default", "Default mesh type")

FBuildingBlockVariant UBuildingBlockDefinition::GetVariantByTag(FGameplayTag VariantTag) const
{
	for (const FBuildingBlockVariant& Variant : Variants)
	{
		if (Variant.VariantTag.MatchesTagExact(VariantTag))
		{
			return Variant;
		}
	}
	return FBuildingBlockVariant();
}

bool UBuildingBlockDefinition::HaveVariant(FGameplayTag VariantTag) const
{
	for (const FBuildingBlockVariant& Variant : Variants)
	{
		if (Variant.VariantTag.MatchesTagExact(VariantTag))
		{
			return true;
		}
	}
	return false;
}
