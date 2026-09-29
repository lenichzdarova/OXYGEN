// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItemRegistry.h"
#include "Components/ActorComponent.h"
#include "InventoryPlacementManagerComponent.generated.h"

class ULyraInventoryItemDefinition;

USTRUCT(BlueprintType)
struct FItemPlacementRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 Position = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite)
	int32 Rotation = 0;

	UPROPERTY(BlueprintReadWrite)
	bool bAllowAutoAdjust = true;
};

USTRUCT(BlueprintType)
struct FPlacementManagerChangeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UInventoryPlacementManagerComponent> SenderComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	FGuid ItemId;
};

UCLASS(Abstract, BlueprintType, ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class OXYGEN_API UInventoryPlacementManagerComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class ULyraInventoryManagerComponent;

public:
	UInventoryPlacementManagerComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory|Placement")
	virtual bool CanPlaceItem(FGuid ItemId, FItemPlacementRequest Request);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Placement")
	virtual bool CanPlaceDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest Request);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Placement")
	virtual bool HasItem(FGuid ItemId) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Placement")
	virtual TArray<FGuid> GetPlacedItems() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Placement")
	virtual const TArray<FItemData>& GetSlots() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Placement")
	virtual FItemData GetSlotData(int32 Index) const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Placement")
	virtual bool CanReplaceItem(FGuid Old, FGuid New) const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Placement")
	virtual int32 GetItemPosition(FGuid ItemId) const;

protected:
	virtual bool AddItem(FGuid ItemId, FItemPlacementRequest Request);
	virtual bool RemoveItem(FGuid ItemId);
	virtual bool MoveItem(FGuid ItemId, FItemPlacementRequest Request);

	UInventoryItemRegistry* GetRegistry() const;
	FItemData GetItemData(FGuid ItemId) const;
	TSubclassOf<ULyraInventoryItemDefinition> GetItemDef(FGuid ItemId) const;
	bool HasAuthority() const;
	virtual void NotifyChanged(FGuid ItemId);
};


