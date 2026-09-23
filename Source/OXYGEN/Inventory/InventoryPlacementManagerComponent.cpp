#include "InventoryPlacementManagerComponent.h"

#include "InventoryGameplayTags.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "LyraGameplayTags.h"

UInventoryPlacementManagerComponent::UInventoryPlacementManagerComponent()
{
	SetIsReplicatedByDefault(true);
}

bool UInventoryPlacementManagerComponent::CanPlaceItem(FGuid ItemId, FItemPlacementRequest Request)
{
	const TSubclassOf<ULyraInventoryItemDefinition> ItemDef = GetItemDef(ItemId);
	if (ItemDef == nullptr)
	{
		return false;
	}

	return CanPlaceDefinition(ItemDef, Request);
}

bool UInventoryPlacementManagerComponent::CanPlaceDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest Request)
{
	return ItemDef != nullptr;
}

bool UInventoryPlacementManagerComponent::AddItem(FGuid ItemId, FItemPlacementRequest Request)
{
	return false;
}

bool UInventoryPlacementManagerComponent::RemoveItem(FGuid ItemId)
{
	return false;
}

bool UInventoryPlacementManagerComponent::MoveItem(FGuid ItemId, FItemPlacementRequest Request)
{
	return false;
}

void UInventoryPlacementManagerComponent::Server_AddItem_Implementation(FGuid ItemId, FItemPlacementRequest Request)
{
	AddItem(ItemId, Request);
}

void UInventoryPlacementManagerComponent::Server_RemoveItem_Implementation(FGuid ItemId)
{
	RemoveItem(ItemId);
}

void UInventoryPlacementManagerComponent::Server_MoveItem_Implementation(FGuid ItemId, FItemPlacementRequest Request)
{
	MoveItem(ItemId, Request);
}

bool UInventoryPlacementManagerComponent::HasItem(FGuid ItemId) const
{
	return false;
}

TArray<FGuid> UInventoryPlacementManagerComponent::GetPlacedItems() const
{
	return {};
}

UInventoryItemRegistry* UInventoryPlacementManagerComponent::GetRegistry() const
{
	const UWorld* World = GetWorld();
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<UInventoryItemRegistry>() : nullptr;
}

FItemData UInventoryPlacementManagerComponent::GetItemData(FGuid ItemId) const
{
	if (UInventoryItemRegistry* Registry = GetRegistry())
	{
		return Registry->MakeData(ItemId);
	}

	return FItemData();
}

TSubclassOf<ULyraInventoryItemDefinition> UInventoryPlacementManagerComponent::GetItemDef(FGuid ItemId) const
{
	if (UInventoryItemRegistry* Registry = GetRegistry())
	{
		return Registry->GetItemDef(ItemId);
	}

	return nullptr;
}

bool UInventoryPlacementManagerComponent::HasAuthority() const
{
	const AActor* Owner = GetOwner();
	return Owner && Owner->HasAuthority();
}

void UInventoryPlacementManagerComponent::NotifyChanged(FGuid ItemId)
{
	FPlacementManagerChangeMessage Message;
	Message.SenderComponent = this;
	Message.ItemId = ItemId;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(InventoryGameplayTags::Inventory_Placement_Message_GridChanged, Message);
}