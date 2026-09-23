// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemRegistry.h"
#include "LyraInventoryItemInstance.h"

FGuid UInventoryItemRegistry::RegisterNewItem(TSubclassOf<ULyraInventoryItemDefinition> NewItemClass)
{
	check(NewItemClass != nullptr);	
	
	ULyraInventoryItemInstance* Instance = NewObject<ULyraInventoryItemInstance>(this); 
	Instance->SetItemDef(NewItemClass);
	for (ULyraInventoryItemFragment* Fragment : GetDefault<ULyraInventoryItemDefinition>(NewItemClass)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(Instance);
		}
	}	
	
	FGuid Guid = FGuid::NewGuid();
	ItemRegistry.Add(Guid, Instance);
	return Guid;
}

void UInventoryItemRegistry::UnregisterItem(FGuid ItemId)
{
	ItemRegistry.Remove(ItemId);
}
