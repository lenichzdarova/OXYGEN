// Fill out your copyright notice in the Description page of Project Settings.


#include "TetrisGrid.h"


// Sets default values for this component's properties
UTetrisGrid::UTetrisGrid()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;		
	GridCells.AddDefaulted(Height * Width);
}

void UTetrisGrid::BeginPlay()
{
	Super::BeginPlay();
}

void UTetrisGrid::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

//GPT

//поиск первого свободного плейсмента из пула и создание нового, если не найдено


bool UTetrisGrid::TryAddItem(UInventoryItemInstance* ItemInstance)
{	
	if (!ItemInstance || !HaveEnoughSpaceWithoutGeometryCheck(ItemInstance))
	{
		return false;
	}
	
	return TryOptimizeGridForItem(ItemInstance);
}



int32 UTetrisGrid::AcquirePlacementIndex()
{
	for (int32 i = 0; i < Placements.Num(); ++i)
	{
		if (Placements[i].ItemInstance == nullptr)
		{
			return i;
		}
	}
	return Placements.AddDefaulted();
}

bool UTetrisGrid::TryOptimizeGridForItem(	UInventoryItemInstance* NewItem)
{
	if (!NewItem)
	{
		return false;
	}

	const TArray<FItemPlacement> OldPlacements = Placements;

	TArray<UInventoryItemInstance*> Items;

	for (const FItemPlacement& Placement : Placements)
	{
		if (Placement.ItemInstance)
		{
			Items.Add(Placement.ItemInstance);
		}
	}

	Items.Add(NewItem);

	Items.Sort(
		[](const UInventoryItemInstance& A,
		   const UInventoryItemInstance& B)
		{
			const auto* IconA =	A.FindFragmentByClass<UInventoryFragment_InventoryIcon>();

			const auto* IconB = B.FindFragmentByClass<UInventoryFragment_InventoryIcon>();

			const int32 AreaA = IconA ? IconA->ItemSize.X * IconA->ItemSize.Y : 1;

			const int32 AreaB =	IconB ? IconB->ItemSize.X * IconB->ItemSize.Y : 1;

			return AreaA > AreaB;
		});

	ClearGrid();

	if (PackItems(Items))
	{
		return true;
	}

	Placements = OldPlacements;
	RebuildGridCells();

	return false;
}

bool UTetrisGrid::PackItems( const TArray<UInventoryItemInstance*>& Items)
{
	return TryPackItemRecursive( Items,	0);
}

bool UTetrisGrid::TryPackItemRecursive( const TArray<UInventoryItemInstance*>& Items, int32 ItemIndex)
{
	if (ItemIndex >= Items.Num())
	{
		return true;
	}

	UInventoryItemInstance* Item = Items[ItemIndex];

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			const FIntPoint Position(X, Y);

			if (!CanPlaceItemAt( Item, Position))
			{
				continue;
			}

			TryPlaceItemAt(	Item, Position);

			if (TryPackItemRecursive( Items, ItemIndex + 1))
			{
				return true;
			}

			RemovePlacedItem(Item);
		}
	}

	return false;
}

bool UTetrisGrid::CanPlaceItemAt( UInventoryItemInstance* Item, FIntPoint Position) const
{
	if (!Item)
	{
		return false;
	}

	const auto* Icon = Item->FindFragmentByClass<UInventoryFragment_InventoryIcon>();

	if (!Icon)
	{
		return false;
	}

	const FIntPoint Size = Icon->ItemSize;

	if (Position.X < 0 || Position.Y < 0 || Position.X + Size.X > Width || Position.Y + Size.Y > Height)
	{
		return false;
	}

	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			const int32 CellIndex = GetCellIndex( Position.X + X, Position.Y + Y);
			if (GridCells[CellIndex].IsOccupied())
			{
				return false;
			}
		}
	}

	return true;
}

bool UTetrisGrid::TryPlaceItemAt( UInventoryItemInstance* Item, FIntPoint Position)
{
	if (!CanPlaceItemAt( Item, Position))
	{
		return false;
	}

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
			const int32 CellIndex = GetCellIndex( Position.X + X, Position.Y + Y);
			GridCells[CellIndex].ItemPlacementIndex = PlacementIndex;
		}
	}

	return true;
}

void UTetrisGrid::RemovePlacedItem(	UInventoryItemInstance* Item)
{
	if (!Item)
	{
		return;
	}

	for (int32 PlacementIndex = 0;  PlacementIndex < Placements.Num(); ++PlacementIndex)
	{
		FItemPlacement& Placement =	Placements[PlacementIndex];

		if (Placement.ItemInstance != Item)
		{
			continue;
		}

		const FIntPoint Size =	Placement.GetSize();

		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			for (int32 X = 0; X < Size.X; ++X)
			{
				const int32 CellIndex =	GetCellIndex( Placement.InitPosition.X + X,	Placement.InitPosition.Y + Y);

				GridCells[CellIndex].ItemPlacementIndex = INDEX_NONE;
			}
		}
		Placement.SetDefaultValues();
		return;
	}
}

void UTetrisGrid::ClearGrid()
{
	for (FInventoryGridCell& Cell : GridCells)
	{
		Cell.ItemPlacementIndex = INDEX_NONE;
	}

	for (FItemPlacement& Placement : Placements)
	{
		Placement.SetDefaultValues();
	}
}

void UTetrisGrid::RebuildGridCells()
{
	for (FInventoryGridCell& Cell : GridCells)
	{
		Cell.ItemPlacementIndex = INDEX_NONE;
	}

	for (int32 PlacementIndex = 0; PlacementIndex < Placements.Num(); ++PlacementIndex)
	{
		const FItemPlacement& Placement = Placements[PlacementIndex];

		if (!Placement.ItemInstance)
		{
			continue;
		}

		const FIntPoint Size = Placement.GetSize();

		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			for (int32 X = 0; X < Size.X; ++X)
			{
				const int32 CellIndex = GetCellIndex( Placement.InitPosition.X + X, Placement.InitPosition.Y + Y);

				GridCells[CellIndex].ItemPlacementIndex = PlacementIndex;
			}
		}
	}
}

int32 UTetrisGrid::GetCellIndex( int32 X, int32 Y) const
{
	return Y * Width + X;
}

FItemPlacement* UTetrisGrid::FindPlacement( UInventoryItemInstance* Item)
{
	for (FItemPlacement& Placement : Placements)
	{
		if (Placement.ItemInstance == Item)
		{
			return &Placement;
		}
	}

	return nullptr;
}

bool UTetrisGrid::HaveEnoughSpaceWithoutGeometryCheck(UInventoryItemInstance* ItemInstance)
{
	return GetEmptyCellCount() >= GetItemSizeInCells(ItemInstance);
}

int32 UTetrisGrid::GetEmptyCellCount()
{
	return GridCells.Num() - GetBusyCellCount();
}

int32 UTetrisGrid::GetBusyCellCount()
{
	int32 Count = 0;
	for (const FInventoryGridCell& Cell : GridCells)
	{
		if (Cell.IsOccupied())
		{
			Count++;
		}
	}
	return Count;
}

int32 UTetrisGrid::GetItemSizeInCells(UInventoryItemInstance* ItemInstance)
{
	const UInventoryFragment_InventoryIcon* IconFragment = ItemInstance->FindFragmentByClass<UInventoryFragment_InventoryIcon>();
	return IconFragment->ItemSize.X * IconFragment->ItemSize.Y;
}

