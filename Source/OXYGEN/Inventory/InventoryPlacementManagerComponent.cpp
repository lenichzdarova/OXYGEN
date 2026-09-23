// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryPlacementManagerComponent.h"

bool UInventoryPlacementManagerComponent::CanPlaceItem(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest Request)
{
	return true;
}

void UInventoryPlacementManagerComponent::Server_MoveItemToSlot_Implementation(ULyraInventoryItemInstance* ItemInstance, FItemPlacementRequest Request)
{	
}

void UInventoryPlacementManagerComponent::Server_RemoveItem_Implementation(ULyraInventoryItemInstance* ItemInstance)
{
}

void UInventoryPlacementManagerComponent::Server_AddItem_Implementation(ULyraInventoryItemInstance* ItemInstance,
                                                                        FItemPlacementRequest Request)
{
}

