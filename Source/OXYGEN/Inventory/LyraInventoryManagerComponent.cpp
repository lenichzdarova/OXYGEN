// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraInventoryManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "LyraAbilitySystemComponent.h"
#include "InventoryGameplayTags.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "InventoryItemRegistry.h"
#include "ItemHoldingContainerRegistry.h"
#include "LyraInventoryItemDefinition.h"
#include "LyraPlayerState.h"
#include "NativeGameplayTags.h"
#include "Abilities/GameplayAbility_ItemDrop.h"
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

	if (GetOwner())
	{
		PlacementManager = GetOwner()->FindComponentByClass<UInventoryPlacementManagerComponent>();
	}
}

void ULyraInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, Items);
	//DOREPLIFETIME_CONDITION(ThisClass, Items, COND_OwnerOnly);
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

	UGameplayMessageSubsystem::Get(this).BroadcastMessage(Channel, Message);
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

bool ULyraInventoryManagerComponent::CanPlaceItem(FGuid ItemId, FItemPlacementRequest PlacementRequest) const
{
	if (!ItemId.IsValid())
	{
		return false;
	}

	if (!PlacementManager)
	{
		return !HasItem(ItemId);
	}

	return PlacementManager->CanPlaceItem(ItemId, PlacementRequest);
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

	if (PlacementManager)
	{
		if (!PlacementManager->CanPlaceItem(ItemId, PlacementRequest))
		{			
			auto ItemData = PlacementManager->GetSlotData(PlacementRequest.Position);
			if (PlacementManager->CanReplaceItem(ItemData.ItemId, ItemId))
			{
				auto ItemHoldingRegistry = GetWorld()->GetSubsystem<UItemHoldingContainerRegistry>();
				return ItemHoldingRegistry->SwapItems(ItemData.ItemId, ItemId);
			}
			return false;
		}

		if (!PlacementManager->AddItem(ItemId, PlacementRequest))
		{			
			return false;
		}
	}	
	
	Items.Add(ItemId);
	NotifyItemChanged(ItemId, InventoryGameplayTags::Inventory_Message_ItemAdded);	
	auto ItemHoldingRegistry = GetWorld()->GetSubsystem<UItemHoldingContainerRegistry>();
	ItemHoldingRegistry->Claim(ItemId, this);
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
	auto ItemHoldingRegistry = GetWorld()->GetSubsystem<UItemHoldingContainerRegistry>();
	ItemHoldingRegistry->Release(ItemId, this);
	return true;
}

bool ULyraInventoryManagerComponent::MoveItem(FGuid ItemId, FItemPlacementRequest PlacementRequest)
{
	if (!HasAuthority() || !HasItem(ItemId) || !PlacementManager)
	{		
		return false;
	}

	return PlacementManager->MoveItem(ItemId, PlacementRequest);
}

void ULyraInventoryManagerComponent::DropItem(FGuid ItemId)
{
	if (!ItemId.IsValid() || !HasItem(ItemId))
	{
		return;
	}

	AActor* OwnerActor = GetOwner();
	
	APawn* Pawn = Cast<APawn>(GetOwner());
	ALyraPlayerState* PlayerState = Pawn ? Pawn->GetPlayerState<ALyraPlayerState>() : nullptr;
	UAbilitySystemComponent* ASC = PlayerState ? PlayerState->GetLyraAbilitySystemComponent() : nullptr;
	/*UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor);
	if (!ASC)
	{
		return;
	}*/

	FGameplayEventData Payload;
	Payload.EventTag = TAG_Ability_Inventory_Drop;
	Payload.Instigator = OwnerActor;
	Payload.Target = OwnerActor;

	FOxygenDropItemTargetData* DropData = new FOxygenDropItemTargetData();
	DropData->ItemId = ItemId;

	FGameplayAbilityTargetDataHandle TargetData;
	TargetData.Add(DropData);
	Payload.TargetData = TargetData;

	auto Result = ASC->HandleGameplayEvent(Payload.EventTag, &Payload);		
}

void ULyraInventoryManagerComponent::Server_AddItem_Implementation(FGuid ItemId, FItemPlacementRequest PlacementRequest)
{
	AddItem(ItemId, PlacementRequest);
}

void ULyraInventoryManagerComponent::Server_RemoveItem_Implementation(FGuid ItemId)
{
	RemoveItem(ItemId);
}

void ULyraInventoryManagerComponent::Server_MoveItem_Implementation(FGuid ItemId, FItemPlacementRequest PlacementRequest)
{
	MoveItem(ItemId, PlacementRequest);
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

const TArray<FItemData>& ULyraInventoryManagerComponent::GetSlots() const
{
	static const TArray<FItemData> Empty;
	return PlacementManager ? PlacementManager->GetSlots() : Empty;
}

FItemData ULyraInventoryManagerComponent::GetSlotData(int32 Index) const
{
	return PlacementManager ? PlacementManager->GetSlotData(Index) : FItemData();
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

	UInventoryItemRegistry* Registry = GetRegistry();
	if (!Registry)
	{
		return false;
	}

	for (const FGuid& ItemId : Items)
	{
		if (ToDestroy.Num() >= NumToConsume)
		{
			break;
		}

		if (Registry->GetItemDef(ItemId) == ItemDef)
		{
			ToDestroy.Add(ItemId);
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
	
}

//Inventory container interface
void ULyraInventoryManagerComponent::Extract_Implementation(FGuid ItemId)
{
	RemoveItem(ItemId);
}

bool ULyraInventoryManagerComponent::CanInsertItem_Implementation(FGuid Old, FGuid New)
{	
	bool Result = true;
	if (PlacementManager)
	{
		Result = PlacementManager->CanReplaceItem(Old, New);
	}
	return Result;
}

void ULyraInventoryManagerComponent::InsertItem_Implementation(FGuid Old, FGuid New)
{
	FItemPlacementRequest PlacementRequest;
	PlacementRequest.Position = PlacementManager->GetItemPosition(Old);
	Extract_Implementation(Old);	
	AddItem(New, PlacementRequest);
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


