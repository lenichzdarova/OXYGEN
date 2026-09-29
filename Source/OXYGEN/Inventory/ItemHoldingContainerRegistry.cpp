// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemHoldingContainerRegistry.h"
#include "InventoryGameplayTags.h"

bool UItemHoldingContainerRegistry::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->GetNetMode() != NM_Client;
}

void UItemHoldingContainerRegistry::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);	
}

void UItemHoldingContainerRegistry::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);	
}

void UItemHoldingContainerRegistry::Deinitialize()
{	
	Locations.Reset();
	Super::Deinitialize();
}

UObject* UItemHoldingContainerRegistry::FindHolder(FGuid ItemId) const
{
	if (auto Found = Locations.Find(ItemId))
	{
		return *Found;
	}
	return nullptr;
}

void UItemHoldingContainerRegistry::Claim(FGuid ItemId, UObject* NewHolder)
{
	if (!ItemId.IsValid() || !NewHolder)
	{
		return;
	}

	if (UObject* CurrentHolder = FindHolder(ItemId))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Item %s is already held by %s"), *ItemId.ToString(), *CurrentHolder->GetName()));
		if (CurrentHolder != NewHolder)
		{
			IInventoryItemContainer::Execute_Extract(CurrentHolder, ItemId);
		}
	}

	Locations.Add(ItemId, NewHolder);
}

void UItemHoldingContainerRegistry::Release(FGuid ItemId, UObject* Holder)
{
	if (FindHolder(ItemId) == Holder)
	{
		Locations.Remove(ItemId);
	}
}

bool UItemHoldingContainerRegistry::SwapItems(FGuid ItemA, FGuid ItemB)
{
	UObject* HolderA = FindHolder(ItemA);
	UObject* HolderB = FindHolder(ItemB);
	if (!HolderA || !HolderB)
	{
		return false;
	}

	if (!IInventoryItemContainer::Execute_CanInsertItem(HolderA, ItemA, ItemB) ||
		!IInventoryItemContainer::Execute_CanInsertItem(HolderB, ItemB, ItemA))
	{
		return false;
	}
	
	Release(ItemA, HolderA);
	Release(ItemB, HolderB);

	IInventoryItemContainer::Execute_InsertItem(HolderA, ItemA, ItemB);
	IInventoryItemContainer::Execute_InsertItem(HolderB, ItemB, ItemA);

	Locations.Add(ItemA, HolderB);
	Locations.Add(ItemB, HolderA);
	return true;
}







