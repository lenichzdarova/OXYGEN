#include "LoadoutManagerComponent.h"

#include "Equipment/LyraEquipmentInstance.h"
#include "Equipment/LyraEquipmentManagerComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Inventory/InventoryFragment_LoadoutItem.h"
#include "Inventory/InventoryGameplayTags.h"
#include "Inventory/LyraInventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"

ULoadoutManagerComponent::ULoadoutManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void ULoadoutManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority() || !Layout)
	{
		return;
	}

	Slots.Reset();
	Equipment.Reset();

	Slots.Reserve(Layout->Slots.Num());
	Equipment.SetNum(Layout->Slots.Num());

	for (const FLoadoutSlotDef& SlotDef : Layout->Slots)
	{
		FLoadoutSlot Slot;
		Slot.SlotId = SlotDef.SlotId;
		Slots.Add(Slot);
	}
}

void ULoadoutManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ThisClass, Slots, COND_OwnerOnly);
}

UInventoryItemRegistry* ULoadoutManagerComponent::GetRegistry() const
{
	const UWorld* World = GetWorld();
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<UInventoryItemRegistry>() : nullptr;
}

ULyraEquipmentManagerComponent* ULoadoutManagerComponent::FindEquipmentManager() const
{
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		return Pawn->FindComponentByClass<ULyraEquipmentManagerComponent>();
	}
	return GetOwner() ? GetOwner()->FindComponentByClass<ULyraEquipmentManagerComponent>() : nullptr;
}

int ULoadoutManagerComponent::GetSlotIndexByID(FGameplayTag SlotId) const
{
	if (!SlotId.IsValid())
	{
		return INDEX_NONE;
	}

	return Slots.IndexOfByPredicate([&SlotId](const FLoadoutSlot& Slot)
	{
		return Slot.SlotId.MatchesTagExact(SlotId);
	});
}

FLoadoutSlotDef ULoadoutManagerComponent::GetSlotDef(FGameplayTag SlotId) const
{
	if (!Layout || !SlotId.IsValid())
	{
		return FLoadoutSlotDef();
	}

	const FLoadoutSlotDef* Found = Layout->Slots.FindByPredicate([&SlotId](const FLoadoutSlotDef& SlotDef)
	{
		return SlotDef.SlotId.MatchesTagExact(SlotId);
	});

	return Found ? *Found : FLoadoutSlotDef();
}

ULyraEquipmentInstance* ULoadoutManagerComponent::GetEquipment(FGameplayTag SlotId) const
{
	const int32 Index = GetSlotIndexByID(SlotId);
	return Equipment.IsValidIndex(Index) ? Equipment[Index].Get() : nullptr;
}

const UInventoryFragment_LoadoutItem* ULoadoutManagerComponent::FindFragment(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const
{
	if (!ItemDef)
	{
		return nullptr;
	}

	const ULyraInventoryItemFragment* Raw = GetDefault<ULyraInventoryItemDefinition>(ItemDef)->FindFragmentByClass(
		UInventoryFragment_LoadoutItem::StaticClass());

	return Cast<const UInventoryFragment_LoadoutItem>(Raw);
}

bool ULoadoutManagerComponent::MatchesSlot(const UInventoryFragment_LoadoutItem& Fragment, FGameplayTag SlotId) const
{
	const FLoadoutSlotDef SlotDef = GetSlotDef(SlotId);
	if (!SlotDef.SlotId.IsValid() || !Fragment.LoadoutTags.HasAny(SlotDef.AcceptedSets))
	{
		return false;
	}

	if (!Fragment.IsUnique)
	{
		return true;
	}

	for (const FLoadoutSlot& Slot : Slots)
	{
		if (Slot.SlotId.MatchesTagExact(SlotId) || !Slot.Item.IsValid())
		{
			continue;
		}

		const UInventoryFragment_LoadoutItem* Other = FindFragment(Slot.Item.Def);
		if (!Other)
		{
			continue;
		}

		for (const FGameplayTag& ItemTag : Fragment.LoadoutTags)
		{
			if (Other->LoadoutTags.HasTagExact(ItemTag))
			{
				return false;
			}
		}
	}

	return true;
}

bool ULoadoutManagerComponent::CanEquipDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FGameplayTag SlotId) const
{
	const UInventoryFragment_LoadoutItem* Fragment = FindFragment(ItemDef);
	return Fragment && MatchesSlot(*Fragment, SlotId);
}

bool ULoadoutManagerComponent::CanEquip(FGuid ItemId, FGameplayTag SlotId) const
{
	UInventoryItemRegistry* Registry = GetRegistry();
	const TSubclassOf<ULyraInventoryItemDefinition> ItemDef = Registry ? Registry->GetItemDef(ItemId) : nullptr;
	return CanEquipDefinition(ItemDef, SlotId);
}

bool ULoadoutManagerComponent::EquipItem(FGuid ItemId, FGameplayTag SlotId)
{
	if (!HasAuthority() || !CanEquip(ItemId, SlotId))
	{
		return false;
	}

	const int32 Index = GetSlotIndexByID(SlotId);
	UInventoryItemRegistry* Registry = GetRegistry();
	if (Index == INDEX_NONE || !Registry)
	{
		return false;
	}

	const FItemData Data = Registry->MakeData(ItemId);
	const UInventoryFragment_LoadoutItem* Fragment = FindFragment(Data.Def);
	if (!Data.IsValid() || !Fragment)
	{
		return false;
	}

	if (Slots[Index].Item.IsValid())
	{
		RemoveEquipment(SlotId);
	}

	Slots[Index].Item = Data;
	ApplyEquipment(SlotId, ItemId, *Fragment);
	NotifyChanged();
	NotifyItemTaken(ItemId);
	return true;
}

void ULoadoutManagerComponent::Server_EquipItem_Implementation(FGuid ItemId, FGameplayTag SlotId)
{
	EquipItem(ItemId, SlotId);
}

FGuid ULoadoutManagerComponent::UnequipItem(FGameplayTag SlotId)
{
	const int32 Index = GetSlotIndexByID(SlotId);
	if (!HasAuthority() || Index == INDEX_NONE || !Slots[Index].Item.IsValid())
	{
		return FGuid();
	}

	const FGuid RemovedId = Slots[Index].Item.ItemId;
	RemoveEquipment(SlotId);
	Slots[Index].Item = FItemData();
	NotifyChanged();
	return RemovedId;
}

void ULoadoutManagerComponent::Server_UnequipItem_Implementation(FGameplayTag SlotId)
{
	UnequipItem(SlotId);
}

void ULoadoutManagerComponent::ApplyEquipment(FGameplayTag SlotId, FGuid ItemId, const UInventoryFragment_LoadoutItem& Fragment)
{
	RemoveEquipment(SlotId);

	const int32 Index = GetSlotIndexByID(SlotId);
	if (Index == INDEX_NONE || !Fragment.EquipmentDefinition)
	{
		return;
	}

	ULyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager();
	if (!EquipmentManager)
	{
		return;
	}

	ULyraEquipmentInstance* NewEquip = EquipmentManager->EquipItem(Fragment.EquipmentDefinition);
	if (NewEquip)
	{
		NewEquip->SetItemID(ItemId);
	}

	if (!Equipment.IsValidIndex(Index))
	{
		Equipment.SetNum(Index + 1);
	}
	Equipment[Index] = NewEquip;
}

void ULoadoutManagerComponent::RemoveEquipment(FGameplayTag SlotId)
{
	const int32 Index = GetSlotIndexByID(SlotId);
	if (!Equipment.IsValidIndex(Index) || !Equipment[Index])
	{
		return;
	}

	if (ULyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		EquipmentManager->UnequipItem(Equipment[Index]);
	}
	Equipment[Index] = nullptr;
}

void ULoadoutManagerComponent::OnRep_Slots()
{
	NotifyChanged();	
}

void ULoadoutManagerComponent::NotifyChanged()
{	
	FLoadoutChangedMessage Message;
	Message.Component = this;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(	InventoryGameplayTags::Inventory_Loadout_Message_Changed,	Message);
}

void ULoadoutManagerComponent::Extract_Implementation(FGuid ItemId)
{
	for (auto Slot : Slots)
	{
		if (Slot.Item.ItemId == ItemId)
		{
			UnequipItem(Slot.SlotId);
			break;
		}
	}
}
