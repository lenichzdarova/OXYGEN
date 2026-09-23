// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraQuickBarComponent.h"

#include "Equipment/LyraEquipmentDefinition.h"
#include "Equipment/LyraEquipmentInstance.h"
#include "Equipment/LyraEquipmentManagerComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/Pawn.h"
#include "Inventory/InventoryFragment_EquippableItem.h"
#include "Inventory/InventoryItemRegistry.h"
#include "Inventory/LyraInventoryItemInstance.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraQuickBarComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Lyra_QuickBar_Message_SlotsChanged, "Lyra.QuickBar.Message.SlotsChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Lyra_QuickBar_Message_ActiveIndexChanged, "Lyra.QuickBar.Message.ActiveIndexChanged");

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
	if (Slots.Num() < NumSlots)
	{
		Slots.SetNum(NumSlots);
	}

	Super::BeginPlay();
}

void ULyraQuickBarComponent::CycleActiveSlotForward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
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
	int32 NewIndex = ActiveSlotIndex;
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

void ULyraQuickBarComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	const FItemView& SlotItem = Slots[ActiveSlotIndex];
	if (!SlotItem.IsValid())
	{
		return;
	}

	UInventoryItemRegistry* Registry = GetRegistry();
	ULyraInventoryItemInstance* Instance = Registry ? Registry->GetItem(SlotItem.ItemId) : nullptr;
	if (!Instance)
	{
		return;
	}

	if (const UInventoryFragment_EquippableItem* EquipInfo = Instance->FindFragmentByClass<UInventoryFragment_EquippableItem>())
	{
		const TSubclassOf<ULyraEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
		if (EquipDef != nullptr)
		{
			if (ULyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
			{
				EquippedItem = EquipmentManager->EquipItem(EquipDef);
				if (EquippedItem != nullptr)
				{
					EquippedItem->SetInstigator(Instance);
				}
			}
		}
	}
}

void ULyraQuickBarComponent::UnequipItemInSlot()
{
	if (ULyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		if (EquippedItem != nullptr)
		{
			EquipmentManager->UnequipItem(EquippedItem);
			EquippedItem = nullptr;
		}
	}
}

ULyraEquipmentManagerComponent* ULyraQuickBarComponent::FindEquipmentManager() const
{
	if (const APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		return OwnerPawn->FindComponentByClass<ULyraEquipmentManagerComponent>();
	}

	if (const AController* OwnerController = Cast<AController>(GetOwner()))
	{
		if (const APawn* Pawn = OwnerController->GetPawn())
		{
			return Pawn->FindComponentByClass<ULyraEquipmentManagerComponent>();
		}
	}

	return nullptr;
}

void ULyraQuickBarComponent::SetActiveSlotIndex_Implementation(int32 NewIndex)
{
	if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		UnequipItemInSlot();
		ActiveSlotIndex = NewIndex;
		EquipItemInSlot();
		OnRep_ActiveSlotIndex();
	}
}

FItemView ULyraQuickBarComponent::GetActiveSlotView() const
{
	return Slots.IsValidIndex(ActiveSlotIndex) ? Slots[ActiveSlotIndex] : FItemView();
}

FGuid ULyraQuickBarComponent::GetActiveSlotItemId() const
{
	return GetActiveSlotView().ItemId;
}

ULyraInventoryItemInstance* ULyraQuickBarComponent::GetActiveSlotItem() const
{
	UInventoryItemRegistry* Registry = GetRegistry();
	return Registry ? Registry->GetItem(GetActiveSlotItemId()) : nullptr;
}

int32 ULyraQuickBarComponent::GetNextFreeItemSlot() const
{
	for (int32 SlotIndex = 0; SlotIndex < Slots.Num(); ++SlotIndex)
	{
		if (!Slots[SlotIndex].IsValid())
		{
			return SlotIndex;
		}
	}
	return INDEX_NONE;
}

void ULyraQuickBarComponent::Server_AddItemToSlot_Implementation(int32 SlotIndex, FGuid ItemId)
{
	AddItemToSlot(SlotIndex, ItemId);
}

void ULyraQuickBarComponent::Server_RemoveItemFromSlot_Implementation(int32 SlotIndex)
{
	RemoveItemFromSlot(SlotIndex);
}

bool ULyraQuickBarComponent::AddItemToSlot(int32 SlotIndex, FGuid ItemId)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !Slots.IsValidIndex(SlotIndex) || !ItemId.IsValid())
	{
		return false;
	}

	UInventoryItemRegistry* Registry = GetRegistry();
	FItemView View;
	if (!Registry || !Registry->GetView(ItemId, View))
	{
		return false;
	}

	const int32 ExistingIndex = FindItemIndex(ItemId);
	if (ExistingIndex != INDEX_NONE && ExistingIndex != SlotIndex)
	{
		Slots[ExistingIndex].Reset();
	}

	if (Slots[SlotIndex].IsValid() && Slots[SlotIndex].ItemId != ItemId)
	{
		return false;
	}

	Slots[SlotIndex] = View;
	OnRep_Slots();
	return true;
}

FItemView ULyraQuickBarComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	FItemView Result;
	if (!GetOwner() || !GetOwner()->HasAuthority() || !Slots.IsValidIndex(SlotIndex))
	{
		return Result;
	}

	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
		ActiveSlotIndex = -1;
	}

	Result = Slots[SlotIndex];
	Slots[SlotIndex].Reset();
	OnRep_Slots();
	return Result;
}

int32 ULyraQuickBarComponent::FindItemIndex(FGuid ItemId) const
{
	for (int32 Index = 0; Index < Slots.Num(); ++Index)
	{
		if (Slots[Index].ItemId == ItemId)
		{
			return Index;
		}
	}
	return INDEX_NONE;
}

UInventoryItemRegistry* ULyraQuickBarComponent::GetRegistry() const
{
	const UWorld* World = GetWorld();
	const UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
	return GI ? GI->GetSubsystem<UInventoryItemRegistry>() : nullptr;
}

void ULyraQuickBarComponent::OnRep_Slots()
{
	FLyraQuickBarSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Component = this;
	Message.Slots = Slots;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_Lyra_QuickBar_Message_SlotsChanged, Message);
}

void ULyraQuickBarComponent::OnRep_ActiveSlotIndex()
{
	FLyraQuickBarActiveIndexChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Component = this;
	Message.ActiveIndex = ActiveSlotIndex;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_Lyra_QuickBar_Message_ActiveIndexChanged, Message);
}
