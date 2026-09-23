#pragma once

#include "LyraInventoryItemDefinition.h"

#include "InventoryTypes.generated.h"

class ULyraInventoryItemInstance;

USTRUCT(BlueprintType)
struct FItemView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	FGuid ItemId;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TSubclassOf<ULyraInventoryItemDefinition> Def;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 StackCount = 1;

	bool IsValid() const
	{
		return ItemId.IsValid() && Def != nullptr;
	}

	void Reset()
	{
		ItemId.Invalidate();
		Def = nullptr;
		StackCount = 1;
	}
};

USTRUCT(BlueprintType)
struct FItemPlacementRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
	FGuid ItemId;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
	int32 Position = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
	int32 Rotation = 0;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
	bool bAllowAutoAdjust = true;
};

USTRUCT()
struct FInventoryItemRecord
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid ItemId;

	UPROPERTY()
	TObjectPtr<ULyraInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 1;
};
