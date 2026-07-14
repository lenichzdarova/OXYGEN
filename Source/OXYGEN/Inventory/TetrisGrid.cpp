// Fill out your copyright notice in the Description page of Project Settings.

#include "TetrisGrid.h"

UTetrisGrid::UTetrisGrid()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTetrisGrid::BeginPlay()
{
	Super::BeginPlay();
	GridCells.SetNum(FMath::Max(0, Height) * FMath::Max(0, Width));
}

bool UTetrisGrid::CanPlaceItemAt(UInventoryItemInstance* Item, FIntPoint Position) const
{
	if (!HasValidItemSize(Item) || Width <= 0 || Height <= 0 || GridCells.Num() != Width * Height)
	{
		return false;
	}

	const FIntPoint Size = Item->FindFragmentByClass<UInventoryFragment_InventoryIcon>()->ItemSize;
	if (Position.X < 0 || Position.Y < 0 || Position.X + Size.X > Width || Position.Y + Size.Y > Height)
	{
		return false;
	}

	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			const int32 PlacementIndex = GridCells[GetCellIndex(Position.X + X, Position.Y + Y)].ItemPlacementIndex;
			if (PlacementIndex != INDEX_NONE
				&& (!Placements.IsValidIndex(PlacementIndex) || Placements[PlacementIndex].ItemInstance != Item))
			{
				return false;
			}
		}
	}

	return true;
}

bool UTetrisGrid::CanPlaceItemAtCell(UInventoryItemInstance* ItemInstance, int32 CellIndex) const
{
	if (!IsValidCellIndex(CellIndex) || Width <= 0)
	{
		return false;
	}

	return CanPlaceItemAt(ItemInstance, FIntPoint(CellIndex % Width, CellIndex / Width));
}

bool UTetrisGrid::TryPlaceItemAtCellByIndex(UInventoryItemInstance* ItemInstance, int32 CellIndex)
{
	if (!CanPlaceItemAtCell(ItemInstance, CellIndex))
	{
		return false;
	}

	return TrySetItemPosition(ItemInstance, FIntPoint(CellIndex % Width, CellIndex / Width));
}

bool UTetrisGrid::FindFirstAvailablePosition(UInventoryItemInstance* Item, FIntPoint& OutPosition) const
{
	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			const FIntPoint Position(X, Y);
			if (CanPlaceItemAt(Item, Position))
			{
				OutPosition = Position;
				return true;
			}
		}
	}

	return false;
}

bool UTetrisGrid::TrySetItemPosition(UInventoryItemInstance* Item, FIntPoint Position)
{
	if (!CanPlaceItemAt(Item, Position))
	{
		return false;
	}

	RemovePlacedItem(Item);

	const int32 PlacementIndex = AcquirePlacementIndex();
	FItemPlacement& Placement = Placements[PlacementIndex];
	Placement.SetDefaultValues();
	Placement.ItemInstance = Item;
	Placement.InitPosition = Position;

	const FIntPoint Size = Placement.GetSize();
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			GridCells[GetCellIndex(Position.X + X, Position.Y + Y)].ItemPlacementIndex = PlacementIndex;
		}
	}

	return true;
}

bool UTetrisGrid::TryAddItem(UInventoryItemInstance* ItemInstance)
{
	FIntPoint Position;
	return FindFirstAvailablePosition(ItemInstance, Position) && TrySetItemPosition(ItemInstance, Position);
}

bool UTetrisGrid::BuildAutoPlacementLayout(UInventoryItemInstance* Item, TArray<FItemPlacement>& OutPlacements) const
{
	if (!HasValidItemSize(Item) || Width <= 0 || Height <= 0)
	{
		return false;
	}

	TArray<UInventoryItemInstance*> Items;
	Items.Reserve(Placements.Num() + 1);
	TSet<UInventoryItemInstance*> UniqueItems;

	for (const FItemPlacement& Placement : Placements)
	{
		if (Placement.ItemInstance && !UniqueItems.Contains(Placement.ItemInstance))
		{
			UniqueItems.Add(Placement.ItemInstance);
			Items.Add(Placement.ItemInstance);
		}
	}
	if (!UniqueItems.Contains(Item))
	{
		UniqueItems.Add(Item);
		Items.Add(Item);
	}

	Items.Sort([](const UInventoryItemInstance& A, const UInventoryItemInstance& B)
	{
		const UInventoryFragment_InventoryIcon* IconA = A.FindFragmentByClass<UInventoryFragment_InventoryIcon>();
		const UInventoryFragment_InventoryIcon* IconB = B.FindFragmentByClass<UInventoryFragment_InventoryIcon>();
		const int32 AreaA = IconA ? IconA->ItemSize.X * IconA->ItemSize.Y : 0;
		const int32 AreaB = IconB ? IconB->ItemSize.X * IconB->ItemSize.Y : 0;
		return AreaA > AreaB;
	});

	TArray<int32> Cells;
	Cells.Init(INDEX_NONE, Width * Height);
	OutPlacements.Reset();
	if (TryBuildAutoPlacementRecursive(Items, 0, Cells, OutPlacements))
	{
		return true;
	}

	OutPlacements.Reset();
	return false;
}

bool UTetrisGrid::TryBuildAutoPlacementRecursive(const TArray<UInventoryItemInstance*>& Items, int32 ItemIndex, TArray<int32>& Cells, TArray<FItemPlacement>& OutPlacements) const
{
	if (ItemIndex == Items.Num())
	{
		return true;
	}

	UInventoryItemInstance* Item = Items[ItemIndex];
	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			const FIntPoint Position(X, Y);
			if (!CanPlaceItemInCells(Item, Position, Cells))
			{
				continue;
			}

			FItemPlacement& Placement = OutPlacements.AddDefaulted_GetRef();
			Placement.ItemInstance = Item;
			Placement.InitPosition = Position;
			const FIntPoint Size = Placement.GetSize();
			for (int32 CellY = 0; CellY < Size.Y; ++CellY)
			{
				for (int32 CellX = 0; CellX < Size.X; ++CellX)
				{
					Cells[GetCellIndex(Position.X + CellX, Position.Y + CellY)] = ItemIndex;
				}
			}

			if (TryBuildAutoPlacementRecursive(Items, ItemIndex + 1, Cells, OutPlacements))
			{
				return true;
			}

			for (int32 CellY = 0; CellY < Size.Y; ++CellY)
			{
				for (int32 CellX = 0; CellX < Size.X; ++CellX)
				{
					Cells[GetCellIndex(Position.X + CellX, Position.Y + CellY)] = INDEX_NONE;
				}
			}
			OutPlacements.Pop();
		}
	}

	return false;
}

bool UTetrisGrid::TryApplyPlacementLayout(const TArray<FItemPlacement>& NewPlacements)
{
	TArray<FInventoryGridCell> NewCells;
	if (!BuildGridCellsForLayout(NewPlacements, NewCells))
	{
		return false;
	}

	Placements = NewPlacements;
	GridCells = MoveTemp(NewCells);
	return true;
}

void UTetrisGrid::RemovePlacedItem(UInventoryItemInstance* Item)
{
	if (!Item)
	{
		return;
	}

	for (int32 PlacementIndex = 0; PlacementIndex < Placements.Num(); ++PlacementIndex)
	{
		FItemPlacement& Placement = Placements[PlacementIndex];
		if (Placement.ItemInstance != Item)
		{
			continue;
		}

		const FIntPoint Size = Placement.GetSize();
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			for (int32 X = 0; X < Size.X; ++X)
			{
				const int32 XInGrid = Placement.InitPosition.X + X;
				const int32 YInGrid = Placement.InitPosition.Y + Y;
				if (XInGrid >= 0 && XInGrid < Width && YInGrid >= 0 && YInGrid < Height)
				{
					GridCells[GetCellIndex(XInGrid, YInGrid)].ItemPlacementIndex = INDEX_NONE;
				}
			}
		}

		Placement.SetDefaultValues();
		return;
	}
}

int32 UTetrisGrid::AcquirePlacementIndex()
{
	for (int32 Index = 0; Index < Placements.Num(); ++Index)
	{
		if (Placements[Index].ItemInstance == nullptr)
		{
			return Index;
		}
	}

	return Placements.AddDefaulted();
}

int32 UTetrisGrid::GetCellIndex(int32 X, int32 Y) const
{
	return Y * Width + X;
}

bool UTetrisGrid::HasValidItemSize(const UInventoryItemInstance* Item) const
{
	if (!Item)
	{
		return false;
	}

	const UInventoryFragment_InventoryIcon* Icon = Item->FindFragmentByClass<UInventoryFragment_InventoryIcon>();
	return Icon && Icon->ItemSize.X > 0 && Icon->ItemSize.Y > 0;
}

bool UTetrisGrid::CanPlaceItemInCells(UInventoryItemInstance* Item, FIntPoint Position, const TArray<int32>& Cells) const
{
	if (!HasValidItemSize(Item) || Position.X < 0 || Position.Y < 0)
	{
		return false;
	}

	const FIntPoint Size = Item->FindFragmentByClass<UInventoryFragment_InventoryIcon>()->ItemSize;
	if (Position.X + Size.X > Width || Position.Y + Size.Y > Height)
	{
		return false;
	}

	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			if (Cells[GetCellIndex(Position.X + X, Position.Y + Y)] != INDEX_NONE)
			{
				return false;
			}
		}
	}

	return true;
}

bool UTetrisGrid::BuildGridCellsForLayout(const TArray<FItemPlacement>& Layout, TArray<FInventoryGridCell>& OutCells) const
{
	if (Width <= 0 || Height <= 0)
	{
		return false;
	}

	OutCells.SetNum(Width * Height);
	for (FInventoryGridCell& Cell : OutCells)
	{
		Cell.ItemPlacementIndex = INDEX_NONE;
	}

	TSet<UInventoryItemInstance*> UniqueItems;
	for (int32 PlacementIndex = 0; PlacementIndex < Layout.Num(); ++PlacementIndex)
	{
		const FItemPlacement& Placement = Layout[PlacementIndex];
		if (!HasValidItemSize(Placement.ItemInstance) || UniqueItems.Contains(Placement.ItemInstance))
		{
			return false;
		}
		UniqueItems.Add(Placement.ItemInstance);

		const FIntPoint Size = Placement.GetSize();
		if (Placement.InitPosition.X < 0 || Placement.InitPosition.Y < 0
			|| Placement.InitPosition.X + Size.X > Width || Placement.InitPosition.Y + Size.Y > Height)
		{
			return false;
		}

		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			for (int32 X = 0; X < Size.X; ++X)
			{
				FInventoryGridCell& Cell = OutCells[GetCellIndex(Placement.InitPosition.X + X, Placement.InitPosition.Y + Y)];
				if (Cell.IsOccupied())
				{
					return false;
				}
				Cell.ItemPlacementIndex = PlacementIndex;
			}
		}
	}

	return true;
}
