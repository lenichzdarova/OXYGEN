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
	ListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener<FItemTakenMessage>(
		InventoryGameplayTags::Inventory_Message_ItemTaken_ByContainer,
		this,
		&UItemHoldingContainerRegistry::OnItemTaken);
}

void UItemHoldingContainerRegistry::Deinitialize()
{	
	if (ListenerHandle.IsValid())
	{
		
		if (UGameplayMessageSubsystem::HasInstance(this))
		{
			UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
			GameplayMessageSubsystem.UnregisterListener(ListenerHandle);
		}
		ListenerHandle = FGameplayMessageListenerHandle();
	}

	Locations.Reset();
	Super::Deinitialize();
}

TScriptInterface<IInventoryItemContainer> UItemHoldingContainerRegistry::FindContainer(FGuid ItemId) const
{
	if (const TScriptInterface<IInventoryItemContainer>* Found = Locations.Find(ItemId))
	{
		return *Found;
	}
	return TScriptInterface<IInventoryItemContainer>();
}

void UItemHoldingContainerRegistry::OnItemTaken(FGameplayTag Channel, const FItemTakenMessage& Message)
{	
	if (!Message.ItemId.IsValid() || !Message.Container.GetObject())
	{
		return;
	}

	const TScriptInterface<IInventoryItemContainer> Old = FindContainer(Message.ItemId);
	if (Old.GetObject() && Old.GetObject() != Message.Container.GetObject())
	{
		IInventoryItemContainer::Execute_Extract(Old.GetObject(), Message.ItemId);
	}

	Locations.Add(Message.ItemId, Message.Container);
}
