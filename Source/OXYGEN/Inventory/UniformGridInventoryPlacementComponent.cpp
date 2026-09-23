#include "UniformGridInventoryPlacementComponent.h"

#include "Net/UnrealNetwork.h"

UUniformGridInventoryPlacementComponent::UUniformGridInventoryPlacementComponent()
{
	SetIsReplicatedByDefault(true);
}

void UUniformGridInventoryPlacementComponent::BeginPlay()
{
	Super::BeginPlay();
	Slots.SetNum(MaxSlots);
}

void UUniformGridInventoryPlacementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ThisClass, Slots, COND_OwnerOnly);
}

bool UUniformGridInventoryPlacementComponent::CanPlaceItem(FGuid ItemId, FItemPlacementRequest Request)
{
	if (!ItemId.IsValid() || HasItem(ItemId))
	{
		return false;
	}

	if (HasAuthority())
	{
		return Super::CanPlaceItem(ItemId, Request);
	}

	// На клиенте реестра нет. Для превью драга достаточно свободной клетки.
	return CanPlaceDefinition(nullptr, Request);
}

bool UUniformGridInventoryPlacementComponent::CanPlaceDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest Request)
{
	const int32 Target = ResolveTargetPosition(Request);
	return IsSlotIndexValid(Target) && IsSlotEmpty(Target);
}

bool UUniformGridInventoryPlacementComponent::AddItem(FGuid ItemId, FItemPlacementRequest Request)
{
	if (!HasAuthority() || !CanPlaceItem(ItemId, Request))
	{
		return false;
	}

	const FItemData Data = GetItemData(ItemId);
	if (!Data.IsValid())
	{
		return false;
	}

	const int32 Target = ResolveTargetPosition(Request);
	Slots[Target] = Data;
	NotifyChanged(ItemId);
	return true;
}

bool UUniformGridInventoryPlacementComponent::RemoveItem(FGuid ItemId)
{
	if (!HasAuthority())
	{
		return false;
	}

	const int32 Index = FindSlotIndex(ItemId);
	if (Index == INDEX_NONE)
	{
		return false;
	}

	Slots[Index] = FItemData();
	NotifyChanged(ItemId);
	return true;
}

bool UUniformGridInventoryPlacementComponent::MoveItem(FGuid ItemId, FItemPlacementRequest Request)
{
	if (!HasAuthority())
	{
		return false;
	}

	const int32 From = FindSlotIndex(ItemId);
	if (From == INDEX_NONE || !IsSlotIndexValid(Request.Position) || From == Request.Position)
	{
		return false;
	}

	const FItemData Swapped = Slots[Request.Position];
	Slots[Request.Position] = Slots[From];
	Slots[From] = Swapped;

	NotifyChanged(ItemId);
	return true;
}

bool UUniformGridInventoryPlacementComponent::HasItem(FGuid ItemId) const
{
	return FindSlotIndex(ItemId) != INDEX_NONE;
}

TArray<FGuid> UUniformGridInventoryPlacementComponent::GetPlacedItems() const
{
	TArray<FGuid> Result;
	Result.Reserve(Slots.Num());

	for (const FItemData& Slot : Slots)
	{
		if (Slot.ItemId.IsValid())
		{
			Result.Add(Slot.ItemId);
		}
	}

	return Result;
}

FItemData UUniformGridInventoryPlacementComponent::GetSlotData(int32 Index) const
{
	return IsSlotIndexValid(Index) ? Slots[Index] : FItemData();
}

void UUniformGridInventoryPlacementComponent::OnRep_Slots()
{
	NotifyChanged(FGuid());
}

bool UUniformGridInventoryPlacementComponent::IsSlotIndexValid(int32 Index) const
{
	return Slots.IsValidIndex(Index);
}

bool UUniformGridInventoryPlacementComponent::IsSlotEmpty(int32 Index) const
{
	return IsSlotIndexValid(Index) && !Slots[Index].IsValid();
}

int32 UUniformGridInventoryPlacementComponent::FindSlotIndex(FGuid ItemId) const
{
	if (!ItemId.IsValid())
	{
		return INDEX_NONE;
	}

	return Slots.IndexOfByPredicate([&ItemId](const FItemData& Slot)
	{
		return Slot.ItemId == ItemId;
	});
}

int32 UUniformGridInventoryPlacementComponent::FindFirstEmptyPosition() const
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

int32 UUniformGridInventoryPlacementComponent::ResolveTargetPosition(FItemPlacementRequest Request) const
{
	return Request.Position == INDEX_NONE ? FindFirstEmptyPosition() : Request.Position;
}