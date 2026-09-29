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

bool UInventoryPlacementManagerComponent::HasItem(FGuid ItemId) const
{
	return false;
}

TArray<FGuid> UInventoryPlacementManagerComponent::GetPlacedItems() const
{
	return {};
}

const TArray<FItemData>& UInventoryPlacementManagerComponent::GetSlots() const
{
	static const TArray<FItemData> Empty;
	return Empty;
}

FItemData UInventoryPlacementManagerComponent::GetSlotData(int32 Index) const
{
	return FItemData();
}

bool UInventoryPlacementManagerComponent::CanReplaceItem(FGuid Old, FGuid New) const
{
	return true;
}

int32 UInventoryPlacementManagerComponent::GetItemPosition(FGuid ItemId) const
{
	return INDEX_NONE;
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
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		InventoryGameplayTags::Inventory_Placement_Message_GridChanged,
		Message);
}