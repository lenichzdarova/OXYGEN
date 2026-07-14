// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryGrid.h"
#include "TetrisGrid.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OXYGEN_API UTetrisGrid : public UInventoryGrid
{
	GENERATED_BODY()

public:
	UTetrisGrid();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
	int32 Height = 8;

	UPROPERTY(EditAnywhere)
	int32 Width = 6;

	/**
	 * Client-side preview. Does not change the grid and may be called every time
	 * the dragged item changes cell.
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	bool CanPlaceItemAt(UInventoryItemInstance* Item, FIntPoint Position) const;

	/** In a tetris grid, CellIndex identifies the item's upper-left cell. */
	virtual bool CanPlaceItemAtCell(UInventoryItemInstance* ItemInstance, int32 CellIndex) const override;

	/** In a tetris grid, CellIndex identifies the item's upper-left cell. */
	virtual bool TryPlaceItemAtCellByIndex(UInventoryItemInstance* ItemInstance, int32 CellIndex) override;

	
	
	
	
	
	/** Client-side search for a suggested position. Does not change the grid. */
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	bool FindFirstAvailablePosition(UInventoryItemInstance* Item, FIntPoint& OutPosition) const;

	/**
	 * Changes stored position only. Call this on the server after it validates
	 * the position submitted by the client.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	bool TrySetItemPosition(UInventoryItemInstance* Item, FIntPoint Position);

	/**
	 * Builds a complete packing proposal without changing this grid. Intended
	 * for client-side auto-loot and UI calculations.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	bool BuildAutoPlacementLayout(UInventoryItemInstance* Item, TArray<FItemPlacement>& OutPlacements) const;

	/**
	 * Validates and stores a complete layout. The network layer must call this
	 * on the server after it has checked that every item belongs to the owner.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	bool TryApplyPlacementLayout(const TArray<FItemPlacement>& NewPlacements);

	/** Places at the first free position only; it never runs auto-packing. */
	virtual bool TryAddItem(UInventoryItemInstance* ItemInstance) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	void RemovePlacedItem(UInventoryItemInstance* Item);

private:
	int32 AcquirePlacementIndex();
	int32 GetCellIndex(int32 X, int32 Y) const;
	bool HasValidItemSize(const UInventoryItemInstance* Item) const;
	bool CanPlaceItemInCells(UInventoryItemInstance* Item, FIntPoint Position, const TArray<int32>& Cells) const;
	bool TryBuildAutoPlacementRecursive(const TArray<UInventoryItemInstance*>& Items, int32 ItemIndex, TArray<int32>& Cells, TArray<FItemPlacement>& OutPlacements) const;
	bool BuildGridCellsForLayout(const TArray<FItemPlacement>& Layout, TArray<FInventoryGridCell>& OutCells) const;
};
