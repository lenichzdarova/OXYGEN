#include "InventoryPlacementManagerComponent.h"

#include "InventoryItemRegistry.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UInventoryPlacementManagerComponent::UInventoryPlacementManagerComponent()
{
	SetIsReplicatedByDefault(true);
}

bool UInventoryPlacementManagerComponent::CanPlaceItem(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest Request) const
{
	return ItemDef != nullptr;
}

void UInventoryPlacementManagerComponent::Server_AddItem_Implementation(FGuid ItemId, FItemPlacementRequest Request)
{
	AddItem(ItemId, Request);
}

void UInventoryPlacementManagerComponent::Server_RemoveItem_Implementation(FGuid ItemId)
{
	RemoveItem(ItemId);
}

void UInventoryPlacementManagerComponent::Server_MoveItemToSlot_Implementation(FGuid ItemId, FItemPlacementRequest Request)
{
	MoveItemToSlot(ItemId, Request);
}

bool UInventoryPlacementManagerComponent::AddItem(FGuid ItemId, FItemPlacementRequest Request)
{
	return false;
}

bool UInventoryPlacementManagerComponent::RemoveItem(FGuid ItemId)
{
	return false;
}

bool UInventoryPlacementManagerComponent::MoveItemToSlot(FGuid ItemId, FItemPlacementRequest Request)
{
	return false;
}

bool UInventoryPlacementManagerComponent::HasAuthority() const
{
	const AActor* Owner = GetOwner();
	return Owner && Owner->HasAuthority();
}

UInventoryItemRegistry* UInventoryPlacementManagerComponent::GetRegistry() const
{
	const UWorld* World = GetWorld();
	const UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
	return GI ? GI->GetSubsystem<UInventoryItemRegistry>() : nullptr;
}
