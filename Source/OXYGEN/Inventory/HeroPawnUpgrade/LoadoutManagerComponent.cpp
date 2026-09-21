#include "LoadoutManagerComponent.h"

#include "Inventory/LyraInventoryItemInstance.h"
#include "Inventory/InventoryFragment_LoadoutItem.h"
#include "Equipment/LyraEquipmentDefinition.h"
#include "Equipment/LyraEquipmentInstance.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

void FLoadoutEntryList::AddEntry(ULyraInventoryItemInstance* Instance)
{
	
}

void FLoadoutEntryList::RemoveEntry(ULyraInventoryItemInstance* Instance)
{
	
}

ULoadoutManagerComponent::ULoadoutManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void ULoadoutManagerComponent::Server_Equip(FGameplayTag SlotTag, ULyraInventoryItemInstance* ItemInstance)
{
	if (!IsValid(ItemInstance))
	{
		return ;
	}
	
	const UInventoryFragment_LoadoutItem* Frag = ItemInstance->FindFragmentByClass<UInventoryFragment_LoadoutItem>();
	if (!Frag || !Frag->EquipmentDefinition)
	{
		return;
	}

	// Замена существующего
	if (Index != INDEX_NONE)
	{
		if (!EntryList.Entries.IsValidIndex(Index))
		{
			return;
		}
		InternalRemoveEntry(Index);
		InternalAddEntry(ItemInstance, Index);
	}
	
	InternalAddEntry(ItemInstance, INDEX_NONE);
}

v

void ULoadoutManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULoadoutManagerComponent, EntryList);
}

void ULoadoutManagerComponent::Unequip(FGameplayTag SlotTag)
{
	for (auto Element : EntryList.Entries)
	{
		if (Element.SlotTag == SlotTag)
		{
			
			return;
		}
	}	
}


bool ULoadoutManagerComponent::Swap(FGameplayTag SlotTagA, FGameplayTag SlotTagB)
{
	int32 IndexA = INDEX_NONE;
	int32 IndexB = INDEX_NONE;

	for (int32 i = 0; i < EntryList.Entries.Num(); ++i)
	{
		if (EntryList.Entries[i].SlotTag == SlotTagA)
		{
			IndexA = i;
		}
		if (EntryList.Entries[i].SlotTag == SlotTagB)
		{
			IndexB = i;
		}
	}

	if (IndexA == IndexB)
	{
		return true;
	}

	if (!EntryList.Entries.IsValidIndex(IndexA) || !EntryList.Entries.IsValidIndex(IndexB))
	{
		return false;
	}

	EntryList.Entries.Swap(IndexA, IndexB);
	EntryList.MarkItemDirty(EntryList.Entries[IndexA]);
	EntryList.MarkItemDirty(EntryList.Entries[IndexB]);

	NotifyLoadoutChanged();
	return true;
}

ULyraInventoryItemInstance* ULoadoutManagerComponent::GetItemByTag(FGameplayTag SlotTag) const
{
	for (const FLoadoutEntry& Entry : EntryList.Entries)
	{
		if (Entry.SlotTag == SlotTag)
		{
			return Entry.ItemInstance;
		}
	}
	return nullptr;
}

ULyraEquipmentInstance* ULoadoutManagerComponent::GetEquipmentByTag(FGameplayTag SlotTag) const
{
	for (const FLoadoutEntry& Entry : EntryList.Entries)
	{
		if (Entry.SlotTag == SlotTag)
		{
			return Entry.EquipmentInstance;
		}
	}
	return nullptr;
}

bool ULoadoutManagerComponent::InternalAddEntry(ULyraInventoryItemInstance* ItemInstance, int32 AtIndex)
{
	const UInventoryFragment_LoadoutItem* Frag =
		ItemInstance->FindFragmentByClass<UInventoryFragment_LoadoutItem>();
	if (!Frag || !Frag->EquipmentDefinition)
	{
		return false;
	}

	const ULyraEquipmentDefinition* DefCDO =
		GetDefault<ULyraEquipmentDefinition>(Frag->EquipmentDefinition);

	TSubclassOf<ULyraEquipmentInstance> InstanceType = DefCDO->InstanceType;
	if (!InstanceType)
	{
		InstanceType = ULyraEquipmentInstance::StaticClass();
	}

	ULyraEquipmentInstance* NewEquip =
		NewObject<ULyraEquipmentInstance>(GetOwner(), InstanceType);
	NewEquip->SetInstigator(ItemInstance);

	FLoadoutEntry NewEntry;
	NewEntry.SlotTag = Frag->SlotTag;
	NewEntry.ItemInstance = ItemInstance;
	NewEntry.EquipmentInstance = NewEquip;	

	NewEquip->SpawnEquipmentActors(DefCDO->ActorsToSpawn);
	NewEquip->OnEquipped();

	if (AtIndex == INDEX_NONE)
	{
		const int32 NewIndex = EntryList.Entries.Add(NewEntry);
		EntryList.MarkItemDirty(EntryList.Entries[NewIndex]);
	}
	else
	{
		EntryList.Entries.Insert(NewEntry, AtIndex);
		EntryList.MarkArrayDirty();
	}

	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(NewEquip);
	}

	NotifyLoadoutChanged();
	return true;
}

void ULoadoutManagerComponent::InternalRemoveEntry(int32 Index)
{
	if (!EntryList.Entries.IsValidIndex(Index))
	{
		return;
	}

	FLoadoutEntry& Entry = EntryList.Entries[Index];

	if (Entry.EquipmentInstance)
	{
		Entry.EquipmentInstance->OnUnequipped();
		Entry.EquipmentInstance->DestroyEquipmentActors();

		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(Entry.EquipmentInstance);
		}
	}

	EntryList.Entries.RemoveAt(Index);
	EntryList.MarkArrayDirty();
}

void ULoadoutManagerComponent::NotifyLoadoutChanged()
{
	FLoadoutChangedMessage Message;
	Message.Component = this;
	Message.Entries = EntryList.Entries;

	// UGameplayMessageSubsystem::Get(this).BroadcastMessage(YourLoadoutChangedTag, Message);
}

ULyraAbilitySystemComponent* ULoadoutManagerComponent::GetOwnerASC() const
{
	AActor* OwnerActor = GetOwner();
	return OwnerActor
		? OwnerActor->FindComponentByClass<ULyraAbilitySystemComponent>()
		: nullptr;
}

bool ULoadoutManagerComponent::ReplicateSubobjects(
	UActorChannel* Channel,
	FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	bool bWrote = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FLoadoutEntry& Entry : EntryList.Entries)
	{
		if (IsValid(Entry.EquipmentInstance))
		{
			bWrote |= Channel->ReplicateSubobject(Entry.EquipmentInstance, *Bunch, *RepFlags);
		}
	}

	return bWrote;
}

void ULoadoutManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FLoadoutEntry& Entry : EntryList.Entries)
		{
			if (IsValid(Entry.EquipmentInstance))
			{
				AddReplicatedSubObject(Entry.EquipmentInstance);
			}
		}
	}
}

void ULoadoutManagerComponent::RemoveItemFromPlayerInventory(ULyraInventoryItemInstance* InItemInstance)
{	
	
}
