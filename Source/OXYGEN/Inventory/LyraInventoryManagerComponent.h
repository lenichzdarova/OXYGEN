// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "InventoryItemContainer.h"
#include "InventoryPlacementManagerComponent.h"
#include "Components/ActorComponent.h"

#include "LyraInventoryManagerComponent.generated.h"

class UInventoryItemRegistry;
class ULyraInventoryItemDefinition;

USTRUCT(BlueprintType)
struct FLyraInventoryChangeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	FGuid ItemId;
};

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class OXYGEN_API ULyraInventoryManagerComponent : public UActorComponent, public IInventoryItemContainer
{
	GENERATED_BODY()

public:
	ULyraInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool CanAddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool CanPlaceItem(FGuid ItemId, FItemPlacementRequest PlacementRequest) const;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = Inventory)
	void Server_AddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	FGuid AddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest PlacementRequest);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool AddItem(FGuid ItemId, FItemPlacementRequest PlacementRequest);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool RemoveItem(FGuid ItemId);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool MoveItem(FGuid ItemId, FItemPlacementRequest PlacementRequest);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool DestroyItem(FGuid ItemId);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool TransferItemTo(FGuid ItemId, ULyraInventoryManagerComponent* Destination, FItemPlacementRequest PlacementRequest);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = Inventory)
	void Server_AddItem(FGuid ItemId, FItemPlacementRequest PlacementRequest);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = Inventory)
	void Server_RemoveItem(FGuid ItemId);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = Inventory)
	void Server_MoveItem(FGuid ItemId, FItemPlacementRequest PlacementRequest);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool HasItem(FGuid ItemId) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	TArray<FGuid> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	const TArray<FItemData>& GetSlots() const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	FItemData GetSlotData(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	UInventoryPlacementManagerComponent* GetPlacementManager() const { return PlacementManager; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	FGuid FindFirstItemByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	int32 GetTotalItemCountByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool ConsumeItemsByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 NumToConsume);

	// Inventory container Interface
	virtual void Extract_Implementation(FGuid ItemId) override;
	virtual bool CanInsertItem_Implementation(FGuid Old, FGuid New) override;
	virtual void InsertItem_Implementation(FGuid Old, FGuid New) override;

protected:
	UFUNCTION()
	void OnRep_Items();

private:
	UInventoryItemRegistry* GetRegistry() const;
	bool HasAuthority() const;
	int32 FindItemIndex(FGuid ItemId) const;
	void NotifyItemChanged(FGuid ItemId, FGameplayTag Channel);

	UPROPERTY(ReplicatedUsing = OnRep_Items)
	TArray<FGuid> Items;

	UPROPERTY()
	TObjectPtr<UInventoryPlacementManagerComponent> PlacementManager = nullptr;
};
