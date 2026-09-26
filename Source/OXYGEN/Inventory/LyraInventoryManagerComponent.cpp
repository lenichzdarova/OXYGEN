// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraInventoryManagerComponent.h"

#include "InventoryGameplayTags.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "InventoryItemRegistry.h"
#include "LyraInventoryItemDefinition.h"
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
	PlacementManager = GetOwner()->FindComponentByClass<UInventoryPlacementManagerComponent>();
}

void ULyraInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ThisClass, Items, COND_OwnerOnly);
}

UInventoryItemRegistry* ULyraInventoryManagerComponent::GetRegistry() const
{
	const UWorld* World = GetWorld();
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<UInventoryItemRegistry>() : nullptr;
}

bool ULyraInventoryManagerComponent::HasAuthority() const
{
	const AActor* Owner = GetOwner();
	return Owner && Owner->HasAuthority();
}

int32 ULyraInventoryManagerComponent::FindItemIndex(FGuid ItemId) const
{
	return ItemId.IsValid() ? Items.IndexOfByKey(ItemId) : INDEX_NONE;
}

void ULyraInventoryManagerComponent::NotifyItemChanged(FGuid ItemId, FGameplayTag Channel)
{
	FLyraInventoryChangeMessage Message;
	Message.InventoryOwner = this;
	Message.ItemId = ItemId;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(Channel, Message);
}

void ULyraInventoryManagerComponent::Extract_Implementation(FGuid ItemId)
{	
	RemoveItem(ItemId);
}

bool ULyraInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const
{
	if (ItemDef == nullptr)
	{
		return false;
	}

	if (PlacementManager)
	{
		return PlacementManager->CanPlaceDefinition(ItemDef, FItemPlacementRequest());
	}

	return true;
}

void ULyraInventoryManagerComponent::Server_AddItemDefinition_Implementation(TSubclassOf<ULyraInventoryItemDefinition> ItemDef)
{
	AddItemDefinition(ItemDef, FItemPlacementRequest());
}

bool ULyraInventoryManagerComponent::Server_AddItemDefinition_Validate(TSubclassOf<ULyraInventoryItemDefinition> ItemDef)
{
	return ItemDef != nullptr;
}

FGuid ULyraInventoryManagerComponent::AddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest PlacementRequest)
{
	if (!HasAuthority() || !CanAddItemDefinition(ItemDef))
	{
		return FGuid();
	}

	UInventoryItemRegistry* Registry = GetRegistry();
	if (!Registry)
	{
		return FGuid();
	}

	const FGuid ItemId = Registry->CreateItem(ItemDef);
	if (!AddItem(ItemId, PlacementRequest))
	{
		Registry->DestroyItem(ItemId);
		return FGuid();
	}

	return ItemId;
}

bool ULyraInventoryManagerComponent::AddItem(FGuid ItemId, FItemPlacementRequest PlacementRequest)
{
	if (!HasAuthority() || !ItemId.IsValid() || HasItem(ItemId))
	{
		return false;
	}

	UInventoryItemRegistry* Registry = GetRegistry();
	if (!Registry || !Registry->HasItem(ItemId))
	{
		return false;
	}

	if (PlacementManager && !PlacementManager->CanPlaceItem(ItemId, PlacementRequest))
	{
		return false;
	}

	Items.Add(ItemId);
	PlacementManager->AddItem(ItemId, PlacementRequest);
	NotifyItemChanged(ItemId, InventoryGameplayTags::Inventory_Message_ItemAdded);
	NotifyItemTaken(ItemId);
	return true;
}

bool ULyraInventoryManagerComponent::RemoveItem(FGuid ItemId)
{
	if (!HasAuthority())
	{
		return false;
	}

	const int32 Index = FindItemIndex(ItemId);
	if (Index == INDEX_NONE)
	{
		return false;
	}	
	Items.RemoveAt(Index);
	if (PlacementManager)
	{
		PlacementManager->RemoveItem(ItemId);
	}
	NotifyItemChanged(ItemId, InventoryGameplayTags::Inventory_Message_ItemRemoved);
	return true;
}

bool ULyraInventoryManagerComponent::DestroyItem(FGuid ItemId)
{
	if (!RemoveItem(ItemId))
	{
		return false;
	}

	if (UInventoryItemRegistry* Registry = GetRegistry())
	{
		Registry->DestroyItem(ItemId);
	}

	return true;
}

bool ULyraInventoryManagerComponent::TransferItemTo(FGuid ItemId, ULyraInventoryManagerComponent* Destination, FItemPlacementRequest PlacementRequest)
{
	if (!HasAuthority() || Destination == nullptr || Destination == this)
	{
		return false;
	}

	if (!HasItem(ItemId))
	{
		return false;
	}

	if (!RemoveItem(ItemId))
	{
		return false;
	}

	if (!Destination->AddItem(ItemId, PlacementRequest))
	{
		AddItem(ItemId, FItemPlacementRequest());
		return false;
	}

	return true;
}

bool ULyraInventoryManagerComponent::HasItem(FGuid ItemId) const
{
	return FindItemIndex(ItemId) != INDEX_NONE;
}

TArray<FGuid> ULyraInventoryManagerComponent::GetAllItems() const
{
	return Items;
}

FGuid ULyraInventoryManagerComponent::FindFirstItemByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const
{
	UInventoryItemRegistry* Registry = GetRegistry();
	if (!Registry || ItemDef == nullptr)
	{
		return FGuid();
	}

	for (const FGuid& ItemId : Items)
	{
		if (Registry->GetItemDef(ItemId) == ItemDef)
		{
			return ItemId;
		}
	}

	return FGuid();
}

int32 ULyraInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const
{
	UInventoryItemRegistry* Registry = GetRegistry();
	if (!Registry || ItemDef == nullptr)
	{
		return 0;
	}

	int32 Total = 0;
	for (const FGuid& ItemId : Items)
	{
		if (Registry->GetItemDef(ItemId) == ItemDef)
		{
			++Total;
		}
	}

	return Total;
}

bool ULyraInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	if (!HasAuthority() || NumToConsume <= 0)
	{
		return false;
	}

	TArray<FGuid> ToDestroy;
	ToDestroy.Reserve(NumToConsume);

	for (const FGuid& ItemId : Items)
	{
		if (ToDestroy.Num() >= NumToConsume)
		{
			break;
		}

		if (UInventoryItemRegistry* Registry = GetRegistry())
		{
			if (Registry->GetItemDef(ItemId) == ItemDef)
			{
				ToDestroy.Add(ItemId);
			}
		}
	}

	if (ToDestroy.Num() < NumToConsume)
	{
		return false;
	}

	for (const FGuid& ItemId : ToDestroy)
	{
		DestroyItem(ItemId);
	}

	return true;
}

void ULyraInventoryManagerComponent::OnRep_Items()
{
	//NotifyItemChanged(FGuid());
}

//////////////////////////////////////////////////////////////////////
//

// UCLASS(Abstract)
// class ULyraInventoryFilter : public UObject
// {
// public:
// 	virtual bool PassesFilter(ULyraInventoryItemInstance* Instance) const { return true; }
// };

// UCLASS()
// class ULyraInventoryFilter_HasTag : public ULyraInventoryFilter
// {
// public:
// 	virtual bool PassesFilter(ULyraInventoryItemInstance* Instance) const { return true; }
// };


