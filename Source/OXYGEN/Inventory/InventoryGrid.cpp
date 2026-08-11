// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryGrid.h"
//#include "IPropertyTable.h"
#include "Engine/World.h"


bool UInventoryGrid::TryAddItem(UInventoryItemInstance* ItemInstance)
{
	return false;
}

bool UInventoryGrid::CanPlaceItemAtCell(UInventoryItemInstance* ItemInstance, int32 CellIndex) const
{
	return false;
}

bool UInventoryGrid::TryPlaceItemAtCellByIndex(UInventoryItemInstance* ItemInstance, int32 CellIndex)
{
	if (!IsValidCellIndex(CellIndex))
	{
		return false;
	}
	return TryPlaceItemAtCellByReference(ItemInstance, GridCells[CellIndex]);
}

bool UInventoryGrid::TryPlaceItemAtCellByReference(UInventoryItemInstance* ItemInstance, FInventoryGridCell& CellReference)
{	
	return false;
}


FInventoryGridCell* UInventoryGrid::GetCellByIndex(int32 CellIndex)
{
	return GridCells.IsValidIndex(CellIndex) ? &GridCells[CellIndex] : nullptr;
}

const FInventoryGridCell* UInventoryGrid::GetCellByIndex(int32 CellIndex) const
{
	return GridCells.IsValidIndex(CellIndex) ? &GridCells[CellIndex] : nullptr;
}

bool UInventoryGrid::IsValidCellIndex(int32 CellIndex) const
{
	return GridCells.IsValidIndex(CellIndex);
}

int32 UInventoryGrid::FindCellIndex(const FInventoryGridCell& Cell) const
{
	if (GridCells.IsEmpty())
	{
		return INDEX_NONE;
	}

	const UPTRINT FirstCellAddress = reinterpret_cast<UPTRINT>(GridCells.GetData());
	const UPTRINT CellAddress = reinterpret_cast<UPTRINT>(&Cell);
	const UPTRINT CellSize = sizeof(FInventoryGridCell);
	const UPTRINT EndCellAddress = FirstCellAddress + GridCells.Num() * CellSize;
	if (CellAddress < FirstCellAddress || CellAddress >= EndCellAddress || (CellAddress - FirstCellAddress) % CellSize != 0)
	{
		return INDEX_NONE;
	}

	return static_cast<int32>((CellAddress - FirstCellAddress) / CellSize);
}

