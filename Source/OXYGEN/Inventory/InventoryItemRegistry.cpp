// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryItemRegistry.h"

#include "Engine/World.h"
#include "LyraInventoryItemDefinition.h"
#include "LyraInventoryItemInstance.h"

bool UInventoryItemRegistry::IsAuthority() const
{
	const UWorld* World = GetWorld();
	return World && (World->GetNetMode() != NM_Client);
}

FGuid UInventoryItemRegistry::CreateItem(TSubclassOf<ULyraInventoryItemDefinition> ItemDef)
{
	if (!IsAuthority() || ItemDef == nullptr)
	{
		return FGuid();
	}

	ULyraInventoryItemInstance* Instance = NewObject<ULyraInventoryItemInstance>(this);
	Instance->SetItemDef(ItemDef);

	if (const ULyraInventoryItemDefinition* DefCDO = GetDefault<ULyraInventoryItemDefinition>(ItemDef))
	{
		for (ULyraInventoryItemFragment* Fragment : DefCDO->Fragments)
		{
			if (Fragment)
			{
				Fragment->OnInstanceCreated(Instance);
			}
		}
	}

	const FGuid ItemId = FGuid::NewGuid();
	Items.Add(ItemId, Instance);
	return ItemId;
}

void UInventoryItemRegistry::DestroyItem(FGuid ItemId)
{
	if (!IsAuthority() || !ItemId.IsValid())
	{
		return;
	}

	Items.Remove(ItemId);
}

bool UInventoryItemRegistry::HasItem(FGuid ItemId) const
{
	return ItemId.IsValid() && Items.Contains(ItemId);
}

ULyraInventoryItemInstance* UInventoryItemRegistry::GetItem(FGuid ItemId) const
{
	if (!IsAuthority())
	{
		return nullptr;
	}

	if (const TObjectPtr<ULyraInventoryItemInstance>* Found = Items.Find(ItemId))
	{
		return *Found;
	}

	return nullptr;
}

FItemData UInventoryItemRegistry::MakeData(FGuid ItemId) const
{
	FItemData Data;
	if (ULyraInventoryItemInstance* Instance = GetItem(ItemId))
	{
		Data.ItemId = ItemId;
		Data.Def = Instance->GetItemDef();
		Data.StatTags = Instance->GetStatTags();
	}
	return Data;
}

TSubclassOf<ULyraInventoryItemDefinition> UInventoryItemRegistry::GetItemDef(FGuid ItemId) const
{
	if (ULyraInventoryItemInstance* Instance = GetItem(ItemId))
	{
		return Instance->GetItemDef();
	}

	return nullptr;
}
