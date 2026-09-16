// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryPlacementManagerComponent.h"

bool UInventoryPlacementManagerComponent::CanPlaceItem(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest Request)
{
	return true;
}

void UInventoryPlacementManagerComponent::Server_MoveItemToSlot_Implementation(ULyraInventoryItemInstance* ItemInstance, FItemPlacementRequest Request)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Item moved to slot base class"));
}

void UInventoryPlacementManagerComponent::Server_RemoveItem_Implementation(ULyraInventoryItemInstance* ItemInstance)
{
}

void UInventoryPlacementManagerComponent::Server_AddItem_Implementation(ULyraInventoryItemInstance* ItemInstance,
                                                                        FItemPlacementRequest Request)
{
}

