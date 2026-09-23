#include "InventoryItemRegistry.h"

#include "LyraInventoryItemDefinition.h"
#include "LyraInventoryItemInstance.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

bool UInventoryItemRegistry::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

bool UInventoryItemRegistry::IsAuthority() const
{
	const UGameInstance* GI = GetGameInstance();
	const UWorld* World = GI ? GI->GetWorld() : nullptr;
	return World && (World->GetNetMode() != NM_Client);
}

FGuid UInventoryItemRegistry::CreateItem(TSubclassOf<ULyraInventoryItemDefinition> NewItemClass, int32 StackCount, FGuid ExistingId)
{
	if (!IsAuthority() || NewItemClass == nullptr || StackCount <= 0)
	{
		return FGuid();
	}

	ULyraInventoryItemInstance* Instance = NewObject<ULyraInventoryItemInstance>(this);
	Instance->SetItemDef(NewItemClass);

	if (const ULyraInventoryItemDefinition* DefCDO = GetDefault<ULyraInventoryItemDefinition>(NewItemClass))
	{
		for (ULyraInventoryItemFragment* Fragment : DefCDO->Fragments)
		{
			if (Fragment)
			{
				Fragment->OnInstanceCreated(Instance);
			}
		}
	}

	return RegisterInstance(Instance, StackCount, ExistingId);
}

FGuid UInventoryItemRegistry::AdoptInstance(ULyraInventoryItemInstance* Instance, int32 StackCount)
{
	if (!IsAuthority() || !IsValid(Instance) || StackCount <= 0)
	{
		return FGuid();
	}

	if (Instance->GetItemId().IsValid())
	{
		if (FInventoryItemRecord* Existing = ItemRegistry.Find(Instance->GetItemId()))
		{
			Existing->StackCount = StackCount;
			Existing->Instance = Instance;
			return Instance->GetItemId();
		}
	}

	return RegisterInstance(Instance, StackCount, Instance->GetItemId());
}

void UInventoryItemRegistry::DestroyItem(FGuid ItemId)
{
	if (!IsAuthority() || !ItemId.IsValid())
	{
		return;
	}

	ItemRegistry.Remove(ItemId);
}

ULyraInventoryItemInstance* UInventoryItemRegistry::GetItem(FGuid ItemId) const
{
	if (const FInventoryItemRecord* Record = ItemRegistry.Find(ItemId))
	{
		return Record->Instance;
	}
	return nullptr;
}

bool UInventoryItemRegistry::GetView(FGuid ItemId, FItemView& OutView) const
{
	const FInventoryItemRecord* Record = ItemRegistry.Find(ItemId);
	if (!Record || !IsValid(Record->Instance))
	{
		OutView.Reset();
		return false;
	}

	OutView.ItemId = Record->ItemId;
	OutView.Def = Record->Instance->GetItemDef();
	OutView.StackCount = Record->StackCount;
	return true;
}

FItemView UInventoryItemRegistry::MakeView(FGuid ItemId) const
{
	FItemView View;
	GetView(ItemId, View);
	return View;
}

int32 UInventoryItemRegistry::GetStackCount(FGuid ItemId) const
{
	if (const FInventoryItemRecord* Record = ItemRegistry.Find(ItemId))
	{
		return Record->StackCount;
	}
	return 0;
}

void UInventoryItemRegistry::SetStackCount(FGuid ItemId, int32 NewCount)
{
	if (!IsAuthority())
	{
		return;
	}

	if (FInventoryItemRecord* Record = ItemRegistry.Find(ItemId))
	{
		Record->StackCount = FMath::Max(0, NewCount);
		if (Record->StackCount <= 0)
		{
			ItemRegistry.Remove(ItemId);
		}
	}
}

bool UInventoryItemRegistry::Contains(FGuid ItemId) const
{
	return ItemRegistry.Contains(ItemId);
}

FGuid UInventoryItemRegistry::RegisterInstance(ULyraInventoryItemInstance* Instance, int32 StackCount, FGuid ExistingId)
{
	const FGuid Id = ExistingId.IsValid() ? ExistingId : FGuid::NewGuid();
	Instance->SetItemId(Id);

	FInventoryItemRecord Record;
	Record.ItemId = Id;
	Record.Instance = Instance;
	Record.StackCount = StackCount;
	ItemRegistry.Add(Id, Record);
	return Id;
}
