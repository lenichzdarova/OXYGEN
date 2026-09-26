// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemContainer.h"
#include "InventoryGameplayTags.h"

#include "GameFramework/GameplayMessageSubsystem.h"


// Add default functionality here for any IInventoryItemContainer functions that are not pure virtual.
void IInventoryItemContainer::NotifyItemTaken(FGuid ItemId)
{
	UObject* Object = Cast<UObject>(this);
	FItemTakenMessage Message;
	Message.ItemId = ItemId;
	Message.Container.SetObject(Object);

	UGameplayMessageSubsystem::Get(Object).BroadcastMessage(InventoryGameplayTags::Inventory_Message_ItemTaken_ByContainer,	Message);
}
