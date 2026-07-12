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
	
	UPROPERTY()
	TObjectPtr<UInventoryItemInstance> ItemInstance = nullptr;
	
	FIntPoint InitPosition;	
	uint8 Rotation = 0;
	
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
		Rotation = 0;
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
	
	UFUNCTION(BlueprintCallable)
	virtual TArray<FInventoryGridCell> GetGridCells() const { return GridCells; }
	
	UFUNCTION(BlueprintCallable)
	virtual TArray<FItemPlacement> GetPlacements() const { return Placements; }	
};
