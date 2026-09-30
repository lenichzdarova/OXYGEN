// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraQuickBarComponent.h"

#include "InventoryFragment_InventoryIcon.h"
#include "InventoryFragment_QuickBarIcon.h"
#include "InventoryGameplayTags.h"
#include "InventoryItemRegistry.h"
#include "Equipment/LyraEquipmentDefinition.h"
#include "Equipment/LyraEquipmentInstance.h"
#include "Equipment/LyraEquipmentManagerComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/Pawn.h"
#include "Inventory/InventoryFragment_EquippableItem.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraQuickBarComponent)

class FLifetimeProperty;
class ULyraEquipmentDefinition;
ULyraQuickBarComponent::ULyraQuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void ULyraQuickBarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ThisClass, Slots, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, ActiveSlotIndex, COND_OwnerOnly);
}

void ULyraQuickBarComponent::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && Slots.Num() < NumSlots)
	{
		Slots.SetNum(NumSlots);
	}
}

bool ULyraQuickBarComponent::HasAuthority() const
{
	const AActor* Owner = GetOwner();
	return Owner && Owner->HasAuthority();
}

UInventoryItemRegistry* ULyraQuickBarComponent::GetRegistry() const
{
	const UWorld* World = GetWorld();
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<UInventoryItemRegistry>() : nullptr;
}

bool ULyraQuickBarComponent::IsSlotEmpty(int32 SlotIndex) const
{
	return Slots.IsValidIndex(SlotIndex) && !Slots[SlotIndex].IsValid();
}

ULyraEquipmentManagerComponent* ULyraQuickBarComponent::FindEquipmentManager() const
{
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		return Pawn->FindComponentByClass<ULyraEquipmentManagerComponent>();
	}

	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = Controller->GetPawn())
		{
			return Pawn->FindComponentByClass<ULyraEquipmentManagerComponent>();
		}
	}

	return nullptr;
}

void ULyraQuickBarComponent::CycleActiveSlotForward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = OldIndex;
	do
	{
		NewIndex = (NewIndex + 1) % Slots.Num();
		if (Slots[NewIndex].IsValid())
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void ULyraQuickBarComponent::CycleActiveSlotBackward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = OldIndex;
	do
	{
		NewIndex = (NewIndex - 1 + Slots.Num()) % Slots.Num();
		if (Slots[NewIndex].IsValid())
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void ULyraQuickBarComponent::SetActiveSlotIndex_Implementation(int32 NewIndex)
{
	if (!Slots.IsValidIndex(NewIndex) || ActiveSlotIndex == NewIndex)
	{
		return;
	}

	UnequipItemInSlot();
	ActiveSlotIndex = NewIndex;
	EquipItemInSlot();
	NotifyActiveIndexChanged();
}

FItemData ULyraQuickBarComponent::GetActiveSlotItem() const
{
	return Slots.IsValidIndex(ActiveSlotIndex) ? Slots[ActiveSlotIndex] : FItemData();
}

int32 ULyraQuickBarComponent::GetNextFreeItemSlot() const
{
	for (int32 Index = 0; Index < Slots.Num(); ++Index)
	{
		if (IsSlotEmpty(Index))
		{
			return Index;
		}
	}
	return INDEX_NONE;
}

bool ULyraQuickBarComponent::AddItemToSlot(int32 SlotIndex, FGuid ItemId)
{
	if (!HasAuthority() || !ItemId.IsValid() || !IsSlotEmpty(SlotIndex))
	{
		return false;
	}

	UInventoryItemRegistry* Registry = GetRegistry();
	if (!Registry)
	{
		return false;
	}

	const FItemData Data = Registry->MakeData(ItemId);	
	if (!Data.IsValid())
	{
		return false;
	}
	auto DefinitionClass = Data.Def->GetDefaultObject<ULyraInventoryItemDefinition>();
	if (!DefinitionClass->FindFragmentByClass(UInventoryFragment_QuickBarIcon::StaticClass()))
	{
		return false;
	}
	

	const int32 Existing = Slots.IndexOfByPredicate([&ItemId](const FItemData& Slot)
	{
		return Slot.ItemId == ItemId;
	});
	if (Existing != INDEX_NONE && Existing != SlotIndex)
	{
		Slots[Existing] = FItemData();
	}

	Slots[SlotIndex] = Data;
	NotifySlotsChanged();
	return true;
}

void ULyraQuickBarComponent::Server_AddItemToSlot_Implementation(int32 SlotIndex, FGuid ItemId)
{
	AddItemToSlot(SlotIndex, ItemId);
}

FGuid ULyraQuickBarComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	if (!HasAuthority() || !Slots.IsValidIndex(SlotIndex) || !Slots[SlotIndex].IsValid())
	{
		return FGuid();
	}

	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
		ActiveSlotIndex = INDEX_NONE;
		NotifyActiveIndexChanged();
	}

	const FGuid RemovedId = Slots[SlotIndex].ItemId;
	Slots[SlotIndex] = FItemData();
	NotifySlotsChanged();
	return RemovedId;
}

void ULyraQuickBarComponent::EquipItemInSlot()
{
	if (!Slots.IsValidIndex(ActiveSlotIndex) || EquippedItem != nullptr)
	{
		return;
	}

	const FGuid ItemId = Slots[ActiveSlotIndex].ItemId;
	UInventoryItemRegistry* Registry = GetRegistry();
	ULyraInventoryItemInstance* Instance = Registry ? Registry->GetItem(ItemId) : nullptr;
	if (!Instance)
	{
		return;
	}

	const UInventoryFragment_EquippableItem* EquipInfo = Instance->FindFragmentByClass<UInventoryFragment_EquippableItem>();
	if (!EquipInfo || !EquipInfo->EquipmentDefinition)
	{
		return;
	}

	ULyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager();
	if (!EquipmentManager)
	{
		return;
	}

	EquippedItem = EquipmentManager->EquipItem(EquipInfo->EquipmentDefinition);
	if (EquippedItem)
	{
		EquippedItem->SetItemID(ItemId);
	}
}

void ULyraQuickBarComponent::UnequipItemInSlot()
{
	if (ULyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		if (EquippedItem)
		{
			EquipmentManager->UnequipItem(EquippedItem);
			EquippedItem = nullptr;
		}
	}
}

void ULyraQuickBarComponent::OnRep_Slots()
{
	NotifySlotsChanged();
}

void ULyraQuickBarComponent::OnRep_ActiveSlotIndex()
{
	NotifyActiveIndexChanged();
}

void ULyraQuickBarComponent::NotifySlotsChanged()
{
	FLyraQuickBarSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Component = this;

	UGameplayMessageSubsystem::Get(this).BroadcastMessage(InventoryGameplayTags::Inventory_QuickBar_Message_SlotsChanged,	Message);
}

void ULyraQuickBarComponent::NotifyActiveIndexChanged()
{
	FLyraQuickBarActiveIndexChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Component = this;
	Message.ActiveIndex = ActiveSlotIndex;

	UGameplayMessageSubsystem::Get(this).BroadcastMessage(InventoryGameplayTags::Inventory_QuickBar_Message_ActiveIndexChanged, Message);
}

