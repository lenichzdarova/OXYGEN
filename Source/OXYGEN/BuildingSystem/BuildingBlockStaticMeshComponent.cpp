#include "BuildingBlockStaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

UBuildingBlockStaticMeshComponent::UBuildingBlockStaticMeshComponent()
{
	SetIsReplicatedByDefault(true);
	CurrentVariant = Tag_Building_Mesh_Type_Default;
}

void UBuildingBlockStaticMeshComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBuildingBlockStaticMeshComponent, CurrentVariant);
}

void UBuildingBlockStaticMeshComponent::SetBlockDefinition(UBuildingBlockDefinition* InDefinition)
{
	BlockDefinition = InDefinition;
	ApplyCurrentVariant();
}

bool UBuildingBlockStaticMeshComponent::SetVariant(FGameplayTag NewVariant)
{
	if (!BlockDefinition || !BlockDefinition->HaveVariant(NewVariant))
	{
		return false;
	}

	if (CurrentVariant == NewVariant)
	{
		return true;
	}

	CurrentVariant = NewVariant;
	OnRep_CurrentVariant();
	return true;
}

bool UBuildingBlockStaticMeshComponent::HasVariant(FGameplayTag VariantTag) const
{
	return BlockDefinition && BlockDefinition->HaveVariant(VariantTag);
}

void UBuildingBlockStaticMeshComponent::OnRep_CurrentVariant()
{
	ApplyCurrentVariant();
}

void UBuildingBlockStaticMeshComponent::ApplyCurrentVariant()
{
	if (!BlockDefinition)
	{
		SetStaticMesh(nullptr);
		return;
	}

	if (CurrentVariant.IsValid() && BlockDefinition->HaveVariant(CurrentVariant))
	{
		const FBuildingBlockVariant Variant = BlockDefinition->GetVariantByTag(CurrentVariant);
		SetStaticMesh(Variant.Mesh);
		return;
	}

	SetStaticMesh(BlockDefinition->DefaultMesh);
}