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
	// Sets default values for this component's properties
	UTetrisGrid();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
		
	
public:
	UPROPERTY(EditAnywhere)
	int32 Height = 8;
	UPROPERTY(EditAnywhere)
	int32 Width = 6;	

protected:
	TArray<FInventoryGridCell> GridCells;
	TArray<FItemPlacement> Placements;	
	
	//gpt	
	
private:

	//поиск первого свободного плейсмента в пуле, если не наден - создание дефолтного
	int32 AcquirePlacementIndex();

	bool PackItems(	const TArray<UInventoryItemInstance*>& Items);

	bool TryPackItemRecursive(const TArray<UInventoryItemInstance*>& Items,	int32 ItemIndex);

	bool CanPlaceItemAt(UInventoryItemInstance* Item, FIntPoint Position) const;

	bool TryPlaceItemAt( UInventoryItemInstance* Item,	FIntPoint Position);

	void RemovePlacedItem(	UInventoryItemInstance* Item);

	void ClearGrid();

	void RebuildGridCells();

	int32 GetCellIndex(	int32 X, int32 Y) const;

	FItemPlacement* FindPlacement(	UInventoryItemInstance* Item);

public:	
	
	bool TryAddItem(UInventoryItemInstance* ItemInstance) override;
	
protected:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool TryOptimizeGridForItem( UInventoryItemInstance* NewItem);
	
	virtual bool HaveEnoughSpaceWithoutGeometryCheck(UInventoryItemInstance* ItemInstance);
	int32 GetEmptyCellCount();
	int32 GetBusyCellCount();
	int32 GetItemSizeInCells(UInventoryItemInstance* ItemInstance);
};

