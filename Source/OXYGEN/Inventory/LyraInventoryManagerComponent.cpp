// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraInventoryManagerComponent.h"

#include "InventoryItemRegistry.h"
#include "LyraInventoryItemDefinition.h"
#include "LyraInventoryItemInstance.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraInventoryManagerComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Lyra_Inventory_Message_StackChanged, "Lyra.Inventory.Message.StackChanged");

ULyraInventoryManagerComponent::ULyraInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void ULyraInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	PlacementManager = GetOwner() ? GetOwner()->FindComponentByClass<UInventoryPlacementManagerComponent>() : nullptr;
}

void ULyraInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ThisClass, OwnedItems, COND_OwnerOnly);
}

bool ULyraInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount) const
{
	if (ItemDef == nullptr || StackCount <= 0)
	{
		return false;
	}

	if (PlacementManager)
	{
		return PlacementManager->CanPlaceItem(ItemDef, FItemPlacementRequest());
	}

	return true;
}

void ULyraInventoryManagerComponent::Server_AddItemDefinition_Implementation(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount)
{
	AddItemDefinition(ItemDef, StackCount);
}

bool ULyraInventoryManagerComponent::Server_AddItemDefinition_Validate(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount)
{
	return ItemDef != nullptr && StackCount > 0;
}

FGuid ULyraInventoryManagerComponent::AddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount, FItemPlacementRequest PlacementRequest)
{
	if (!HasAuthority() || !CanAddItemDefinition(ItemDef, StackCount))
	{
		return FGuid();
	}

	UInventoryItemRegistry* Registry = GetRegistry();
	if (!Registry)
	{
		return FGuid();
	}

	const FGuid ItemId = Registry->CreateItem(ItemDef, StackCount);
	if (!ItemId.IsValid())
	{
		return FGuid();
	}

	FItemView View;
	if (!Registry->GetView(ItemId, View) || !AddOwnedView(View))
	{
		Registry->DestroyItem(ItemId);
		return FGuid();
	}

	PlacementRequest.ItemId = ItemId;
	if (PlacementManager && !PlacementManager->AddItem(ItemId, PlacementRequest))
	{
		RemoveOwnedItem(ItemId);
		Registry->DestroyItem(ItemId);
		return FGuid();
	}

	BroadcastChangeMessage(View, 0, View.StackCount);
	return ItemId;
}

FGuid ULyraInventoryManagerComponent::AddItemInstance(ULyraInventoryItemInstance* ItemInstance, FItemPlacementRequest PlacementRequest)
{
	if (!HasAuthority() || !IsValid(ItemInstance))
	{
		return FGuid();
	}

	UInventoryItemRegistry* Registry = GetRegistry();
	if (!Registry)
	{
		return FGuid();
	}

	const FGuid ItemId = Registry->AdoptInstance(ItemInstance, 1);
	FItemView View;
	if (!ItemId.IsValid() || !Registry->GetView(ItemId, View))
	{
		return FGuid();
	}

	if (PlacementManager && !PlacementManager->CanPlaceItem(View.Def, PlacementRequest))
	{
		return FGuid();
	}

	if (ContainsItem(ItemId))
	{
		return ItemId;
	}

	if (!AddOwnedView(View))
	{
		return FGuid();
	}

	PlacementRequest.ItemId = ItemId;
	if (PlacementManager && !PlacementManager->AddItem(ItemId, PlacementRequest))
	{
		RemoveOwnedItem(ItemId);
		return FGuid();
	}

	BroadcastChangeMessage(View, 0, View.StackCount);
	return ItemId;
}

void ULyraInventoryManagerComponent::RemoveItem(FGuid ItemId, bool bDestroyRecord)
{
	if (!HasAuthority() || !ItemId.IsValid())
	{
		return;
	}

	FItemView Removed;
	if (!RemoveOwnedItem(ItemId, &Removed))
	{
		return;
	}

	if (PlacementManager)
	{
		PlacementManager->RemoveItem(ItemId);
	}

	if (bDestroyRecord)
	{
		if (UInventoryItemRegistry* Registry = GetRegistry())
		{
			Registry->DestroyItem(ItemId);
		}
	}

	BroadcastChangeMessage(Removed, Removed.StackCount, 0);
}

bool ULyraInventoryManagerComponent::TransferItemTo(FGuid ItemId, ULyraInventoryManagerComponent* Destination, FItemPlacementRequest PlacementRequest)
{
	if (!HasAuthority() || !Destination || Destination == this || !ContainsItem(ItemId))
	{
		return false;
	}

	UInventoryItemRegistry* Registry = GetRegistry();
	FItemView View;
	if (!Registry || !Registry->GetView(ItemId, View))
	{
		return false;
	}

	if (Destination->PlacementManager && !Destination->PlacementManager->CanPlaceItem(View.Def, PlacementRequest))
	{
		return false;
	}

	FItemView Removed;
	if (!RemoveOwnedItem(ItemId, &Removed))
	{
		return false;
	}

	if (PlacementManager)
	{
		PlacementManager->RemoveItem(ItemId);
	}

	if (!Destination->AddOwnedView(View))
	{
		AddOwnedView(Removed);
		if (PlacementManager)
		{
			FItemPlacementRequest RestoreRequest;
			RestoreRequest.ItemId = ItemId;
			PlacementManager->AddItem(ItemId, RestoreRequest);
		}
		return false;
	}

	PlacementRequest.ItemId = ItemId;
	if (Destination->PlacementManager && !Destination->PlacementManager->AddItem(ItemId, PlacementRequest))
	{
		Destination->RemoveOwnedItem(ItemId);
		AddOwnedView(Removed);
		if (PlacementManager)
		{
			FItemPlacementRequest RestoreRequest;
			RestoreRequest.ItemId = ItemId;
			PlacementManager->AddItem(ItemId, RestoreRequest);
		}
		return false;
	}

	BroadcastChangeMessage(Removed, Removed.StackCount, 0);
	Destination->BroadcastChangeMessage(View, 0, View.StackCount);
	return true;
}

FItemView ULyraInventoryManagerComponent::FindFirstItemByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const
{
	for (const FItemView& Item : OwnedItems)
	{
		if (Item.Def == ItemDef)
		{
			return Item;
		}
	}
	return FItemView();
}

int32 ULyraInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const
{
	int32 TotalCount = 0;
	for (const FItemView& Item : OwnedItems)
	{
		if (Item.Def == ItemDef)
		{
			TotalCount += Item.StackCount;
		}
	}
	return TotalCount;
}

bool ULyraInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	if (!HasAuthority() || ItemDef == nullptr || NumToConsume <= 0)
	{
		return false;
	}

	int32 Remaining = NumToConsume;
	TArray<FGuid> ToRemove;

	for (const FItemView& Item : OwnedItems)
	{
		if (Remaining <= 0)
		{
			break;
		}
		if (Item.Def == ItemDef)
		{
			ToRemove.Add(Item.ItemId);
			Remaining -= Item.StackCount;
		}
	}

	if (Remaining > 0)
	{
		return false;
	}

	for (const FGuid& ItemId : ToRemove)
	{
		RemoveItem(ItemId, true);
	}

	return true;
}

bool ULyraInventoryManagerComponent::ContainsItem(FGuid ItemId) const
{
	for (const FItemView& Item : OwnedItems)
	{
		if (Item.ItemId == ItemId)
		{
			return true;
		}
	}
	return false;
}

ULyraInventoryItemInstance* ULyraInventoryManagerComponent::GetItemInstance(FGuid ItemId) const
{
	UInventoryItemRegistry* Registry = GetRegistry();
	return Registry ? Registry->GetItem(ItemId) : nullptr;
}

void ULyraInventoryManagerComponent::OnRep_OwnedItems()
{
	for (const FItemView& Item : OwnedItems)
	{
		BroadcastChangeMessage(Item, 0, Item.StackCount);
	}
}

void ULyraInventoryManagerComponent::BroadcastChangeMessage(const FItemView& Item, int32 OldCount, int32 NewCount)
{
	if (!GetWorld())
	{
		return;
	}

	FLyraInventoryChangeMessage Message;
	Message.InventoryOwner = this;
	Message.Item = Item;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(TAG_Lyra_Inventory_Message_StackChanged, Message);
}

bool ULyraInventoryManagerComponent::AddOwnedView(const FItemView& View)
{
	if (!View.IsValid() || ContainsItem(View.ItemId))
	{
		return false;
	}

	OwnedItems.Add(View);
	return true;
}

bool ULyraInventoryManagerComponent::RemoveOwnedItem(FGuid ItemId, FItemView* OutRemoved)
{
	for (int32 Index = 0; Index < OwnedItems.Num(); ++Index)
	{
		if (OwnedItems[Index].ItemId == ItemId)
		{
			if (OutRemoved)
			{
				*OutRemoved = OwnedItems[Index];
			}
			OwnedItems.RemoveAt(Index);
			return true;
		}
	}
	return false;
}

UInventoryItemRegistry* ULyraInventoryManagerComponent::GetRegistry() const
{
	const UWorld* World = GetWorld();
	const UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
	return GI ? GI->GetSubsystem<UInventoryItemRegistry>() : nullptr;
}

bool ULyraInventoryManagerComponent::HasAuthority() const
{
	const AActor* Owner = GetOwner();
	return Owner && Owner->HasAuthority();
}
