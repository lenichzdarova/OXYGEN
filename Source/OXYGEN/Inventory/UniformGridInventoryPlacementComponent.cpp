// Fill out your copyright notice in the Description page of Project Settings.


#include "UniformGridInventoryPlacementComponent.h"
#include "LyraGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"

UUniformGridInventoryPlacementComponent::UUniformGridInventoryPlacementComponent()
{
	SetIsReplicatedByDefault(true);
	if (GetOwnerRole() == ROLE_Authority)
	{
		Slots.Init(nullptr, MaxSlots);
	}
}

bool UUniformGridInventoryPlacementComponent::CanPlaceItem(TSubclassOf<ULyraInventoryItemDefinition> ItemDef,
                                                           FItemPlacementRequest Request)
{
	bool Result = false;
	if (Request.Position != INDEX_NONE)
	{
		Result = Slots[Request.Position] == nullptr;
	}
	else
	{
		Request.Position = FindFirstEmptyPosition();
		Result = Request.Position != INDEX_NONE;
	}	
	return Result; 
}



void UUniformGridInventoryPlacementComponent::Server_AddItem_Implementation(ULyraInventoryItemInstance* ItemInstance,
                                                      FItemPlacementRequest Request)
{
	int Index = Request.Position == INDEX_NONE? FindFirstEmptyPosition() : Request.Position;
	Slots[Index] = ItemInstance;	
}

void UUniformGridInventoryPlacementComponent::Server_RemoveItem_Implementation(ULyraInventoryItemInstance* ItemInstance)
{
	auto Index = Slots.Find(ItemInstance);
	if (Index != INDEX_NONE)
	{
		Slots[Index] = nullptr;
	}
}

void UUniformGridInventoryPlacementComponent::Server_MoveItemToSlot_Implementation(ULyraInventoryItemInstance* ItemInstance,
	FItemPlacementRequest Request)
{
	auto StartSlot = Slots.Find(ItemInstance);
	ULyraInventoryItemInstance* TargetSlotPreviousItem = Slots[Request.Position];
	Slots[Request.Position] = ItemInstance;
	Slots[StartSlot] = TargetSlotPreviousItem;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Item moved to slot"));
}

void UUniformGridInventoryPlacementComponent::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, Slots);
}

void UUniformGridInventoryPlacementComponent::OnRep_Slots()
{
	NotifySlotsChanged();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Slots changed OnRepFunc"));
}

int UUniformGridInventoryPlacementComponent::FindFirstEmptyPosition()
{
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i] == nullptr)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

void UUniformGridInventoryPlacementComponent::NotifySlotsChanged()
{
	FPlacementManagerChangeMessage Message;
	Message.SenderComponent = this;
	Message.Slots = Slots;
    
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(LyraGameplayTags::Inventory_Placement_Message_GridChanged, Message);
}

