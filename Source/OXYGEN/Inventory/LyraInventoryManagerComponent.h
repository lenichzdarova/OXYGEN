// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "InventoryPlacementManagerComponent.h"
#include "InventoryTypes.h"
#include "Components/ActorComponent.h"

#include "LyraInventoryManagerComponent.generated.h"

class UInventoryItemRegistry;
class ULyraInventoryItemDefinition;
class ULyraInventoryItemInstance;

USTRUCT(BlueprintType)
struct FLyraInventoryChangeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	FItemView Item;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};

UCLASS(BlueprintType)
class ULyraInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULyraInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool CanAddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount = 1) const;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category=Inventory)
	void Server_AddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	FGuid AddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount, FItemPlacementRequest PlacementRequest);

	FGuid AddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount = 1)
	{
		return AddItemDefinition(ItemDef, StackCount, FItemPlacementRequest());
	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	FGuid AddItemInstance(ULyraInventoryItemInstance* ItemInstance, FItemPlacementRequest PlacementRequest);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void RemoveItem(FGuid ItemId, bool bDestroyRecord = false);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	bool TransferItemTo(FGuid ItemId, ULyraInventoryManagerComponent* Destination, FItemPlacementRequest PlacementRequest);

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	TArray<FItemView> GetAllItems() const { return OwnedItems; }

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	FItemView FindFirstItemByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const;

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	int32 GetTotalItemCountByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	bool ConsumeItemsByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 NumToConsume);

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	bool ContainsItem(FGuid ItemId) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	ULyraInventoryItemInstance* GetItemInstance(FGuid ItemId) const;

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	UInventoryPlacementManagerComponent* GetPlacementManager() const { return PlacementManager; }

private:
	UFUNCTION()
	void OnRep_OwnedItems();

	void BroadcastChangeMessage(const FItemView& Item, int32 OldCount, int32 NewCount);
	bool AddOwnedView(const FItemView& View);
	bool RemoveOwnedItem(FGuid ItemId, FItemView* OutRemoved = nullptr);
	UInventoryItemRegistry* GetRegistry() const;
	bool HasAuthority() const;

	UPROPERTY(ReplicatedUsing=OnRep_OwnedItems)
	TArray<FItemView> OwnedItems;

	UPROPERTY()
	TObjectPtr<UInventoryPlacementManagerComponent> PlacementManager = nullptr;
};
