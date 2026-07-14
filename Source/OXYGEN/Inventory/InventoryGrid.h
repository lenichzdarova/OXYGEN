// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryFragment_InventoryIcon.h"
#include "InventoryItemInstance.h"
#include "Components/ActorComponent.h"
#include "InventoryGrid.generated.h"

USTRUCT(Blueprintable)
struct FItemPlacement
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UInventoryItemInstance> ItemInstance = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	FIntPoint InitPosition;
	
	FIntPoint GetSize() const
	{
		FIntPoint Result{1, 1};
		if (ItemInstance && ItemInstance->FindFragmentByClass<UInventoryFragment_InventoryIcon>())
		{
			Result = ItemInstance->FindFragmentByClass<UInventoryFragment_InventoryIcon>()->ItemSize;
		}
		return Result;
	}

	void SetDefaultValues()
	{
		ItemInstance = nullptr;
		InitPosition = FIntPoint(0, 0);
	}
	
	bool IsValid() const
	{
		return ItemInstance != nullptr;
	}
};


USTRUCT(Blueprintable)
struct FInventoryGridCell
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 ItemPlacementIndex = INDEX_NONE;
	
	bool IsOccupied() const { return ItemPlacementIndex != INDEX_NONE; }	
};

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent), Abstract)
class OXYGEN_API UInventoryGrid : public UActorComponent
{
	GENERATED_BODY()	
	
protected:
	TArray<FInventoryGridCell> GridCells;
	TArray<FItemPlacement> Placements;	
	
public:
		
	UFUNCTION(BlueprintCallable)
	virtual bool TryAddItem(UInventoryItemInstance* ItemInstance);

	/** Checks placement using a grid-independent cell identifier. */
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	virtual bool CanPlaceItemAtCell(UInventoryItemInstance* ItemInstance, int32 CellIndex) const;

	/**
	 * Places or moves an item using a grid-independent cell identifier.
	 * Each grid type defines what an index means.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	virtual bool TryPlaceItemAtCellByIndex(UInventoryItemInstance* ItemInstance, int32 CellIndex);

	/** Same operation as TryPlaceItemAtCell, with a stable value handle. */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")	
	virtual bool TryPlaceItemAtCellByReference(UInventoryItemInstance* ItemInstance, FInventoryGridCell& CellReference);

	/** C++ access to the actual cell owned by this grid. */
	FInventoryGridCell* GetCellByIndex(int32 CellIndex);
	const FInventoryGridCell* GetCellByIndex(int32 CellIndex) const;

	/** C++ view of the actual cell storage; Blueprint GetGridCells returns a copy. */
	const TArray<FInventoryGridCell>& GetGridCellsRef() const { return GridCells; }

	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	bool IsValidCellIndex(int32 CellIndex) const;

	UFUNCTION(BlueprintCallable)
	virtual TArray<FInventoryGridCell> GetGridCells() const { return GridCells; }
	
	UFUNCTION(BlueprintCallable)
	virtual TArray<FItemPlacement> GetPlacements() const { return Placements; }	

protected:
	int32 FindCellIndex(const FInventoryGridCell& Cell) const;
};
