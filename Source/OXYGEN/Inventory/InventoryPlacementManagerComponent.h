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

public:
	UInventoryPlacementManagerComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory|Placement")
	virtual bool CanPlaceItem(FGuid ItemId, FItemPlacementRequest Request);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Placement")
	virtual bool CanPlaceDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest Request);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory|Placement")
	virtual bool AddItem(FGuid ItemId, FItemPlacementRequest Request);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory|Placement")
	virtual bool RemoveItem(FGuid ItemId);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory|Placement")
	virtual bool MoveItem(FGuid ItemId, FItemPlacementRequest Request);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory|Placement")
	virtual void Server_AddItem(FGuid ItemId, FItemPlacementRequest Request);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory|Placement")
	virtual void Server_RemoveItem(FGuid ItemId);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory|Placement")
	virtual void Server_MoveItem(FGuid ItemId, FItemPlacementRequest Request);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Placement")
	virtual bool HasItem(FGuid ItemId) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Placement")
	virtual TArray<FGuid> GetPlacedItems() const;

protected:
	UInventoryItemRegistry* GetRegistry() const;
	FItemData GetItemData(FGuid ItemId) const;
	TSubclassOf<ULyraInventoryItemDefinition> GetItemDef(FGuid ItemId) const;
	bool HasAuthority() const;

	virtual void NotifyChanged(FGuid ItemId);
};


