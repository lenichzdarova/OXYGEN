#include "UniformGridInventoryPlacementComponent.h"

#include "InventoryItemRegistry.h"
#include "LyraGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"

UUniformGridInventoryPlacementComponent::UUniformGridInventoryPlacementComponent()
{
	SetIsReplicatedByDefault(true);
}

void UUniformGridInventoryPlacementComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Slots.Num() != MaxSlots)
	{
		Slots.SetNum(MaxSlots);
	}
}

void UUniformGridInventoryPlacementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ThisClass, Slots, COND_OwnerOnly);
}

bool UUniformGridInventoryPlacementComponent::CanPlaceItem(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest Request) const
{
	if (ItemDef == nullptr || Slots.Num() == 0)
	{
		return false;
	}

	if (Request.Position != INDEX_NONE)
	{
		return Slots.IsValidIndex(Request.Position) && !Slots[Request.Position].IsValid();
	}

	return FindFirstEmptyPosition() != INDEX_NONE;
}

bool UUniformGridInventoryPlacementComponent::AddItem(FGuid ItemId, FItemPlacementRequest Request)
{
	if (!HasAuthority() || !ItemId.IsValid())
	{
		return false;
	}

	UInventoryItemRegistry* Registry = GetRegistry();
	FItemView View;
	if (!Registry || !Registry->GetView(ItemId, View))
	{
		return false;
	}

	if (!CanPlaceItem(View.Def, Request))
	{
		return false;
	}

	if (FindItemIndex(ItemId) != INDEX_NONE)
	{
		return false;
	}

	const int32 Index = (Request.Position == INDEX_NONE) ? FindFirstEmptyPosition() : Request.Position;
	if (!Slots.IsValidIndex(Index) || Slots[Index].IsValid())
	{
		return false;
	}

	Slots[Index] = View;
	NotifySlotsChanged();
	return true;
}

bool UUniformGridInventoryPlacementComponent::RemoveItem(FGuid ItemId)
{
	if (!HasAuthority() || !ItemId.IsValid())
	{
		return false;
	}

	const int32 Index = FindItemIndex(ItemId);
	if (Index == INDEX_NONE)
	{
		return false;
	}

	Slots[Index].Reset();
	NotifySlotsChanged();
	return true;
}

bool UUniformGridInventoryPlacementComponent::MoveItemToSlot(FGuid ItemId, FItemPlacementRequest Request)
{
	if (!HasAuthority() || !ItemId.IsValid() || !Slots.IsValidIndex(Request.Position))
	{
		return false;
	}

	const int32 StartIndex = FindItemIndex(ItemId);
	if (StartIndex == INDEX_NONE)
	{
		return false;
	}

	const FItemView Moving = Slots[StartIndex];
	Slots[StartIndex] = Slots[Request.Position];
	Slots[Request.Position] = Moving;
	NotifySlotsChanged();
	return true;
}

bool UUniformGridInventoryPlacementComponent::ContainsItem(FGuid ItemId) const
{
	return FindItemIndex(ItemId) != INDEX_NONE;
}

FItemView UUniformGridInventoryPlacementComponent::GetSlotView(int32 Index) const
{
	return Slots.IsValidIndex(Index) ? Slots[Index] : FItemView();
}

void UUniformGridInventoryPlacementComponent::OnRep_Slots()
{
	NotifySlotsChanged();
}

int32 UUniformGridInventoryPlacementComponent::FindFirstEmptyPosition() const
{
	for (int32 Index = 0; Index < Slots.Num(); ++Index)
	{
		if (!Slots[Index].IsValid())
		{
			return Index;
		}
	}
	return INDEX_NONE;
}

int32 UUniformGridInventoryPlacementComponent::FindItemIndex(FGuid ItemId) const
{
	if (!ItemId.IsValid())
	{
		return INDEX_NONE;
	}

	for (int32 Index = 0; Index < Slots.Num(); ++Index)
	{
		if (Slots[Index].ItemId == ItemId)
		{
			return Index;
		}
	}
	return INDEX_NONE;
}

void UUniformGridInventoryPlacementComponent::NotifySlotsChanged()
{
	FPlacementManagerChangeMessage Message;
	Message.SenderComponent = this;
	Message.Slots = Slots;

	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(World);
		MessageSystem.BroadcastMessage(LyraGameplayTags::Inventory_Placement_Message_GridChanged, Message);
	}
}
