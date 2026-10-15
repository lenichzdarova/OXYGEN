// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingBlockStaticMeshComponent.h"

UBuildingBlockStaticMeshComponent::UBuildingBlockStaticMeshComponent()
{
	if (BlockDefinition)
	{
		SetStaticMesh(BlockDefinition->Mesh);
	}
}

void UBuildingBlockStaticMeshComponent::SetBlockDefinition(UBuildingBlockDefinition* InBlockDefinition)
{
	BlockDefinition = InBlockDefinition;
	OnDefinitionSet();
}

void UBuildingBlockStaticMeshComponent::OnDefinitionSet()
{
	if (BlockDefinition)
	{
		SetStaticMesh(BlockDefinition->Mesh);
	}
	else
	{
		SetStaticMesh(nullptr);
	}
}
