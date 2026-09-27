// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryPlacementManagerComponent.h"
#include "UniformGridInventoryPlacementComponent.generated.h"

UCLASS(BlueprintType, ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class OXYGEN_API UUniformGridInventoryPlacementComponent : public UInventoryPlacementManagerComponent
{
	GENERATED_BODY()

public:
	UUniformGridInventoryPlacementComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Storage")
	int32 MaxSlots = 48;

	virtual bool CanPlaceItem(FGuid ItemId, FItemPlacementRequest Request) override;
	virtual bool CanPlaceDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest Request) override;

	virtual bool HasItem(FGuid ItemId) const override;
	virtual TArray<FGuid> GetPlacedItems() const override;

	virtual const TArray<FItemData>& GetSlots() const override { return Slots; }
	virtual FItemData GetSlotData(int32 Index) const override;

protected:
	virtual bool AddItem(FGuid ItemId, FItemPlacementRequest Request) override;
	virtual bool RemoveItem(FGuid ItemId) override;
	virtual bool MoveItem(FGuid ItemId, FItemPlacementRequest Request) override;

	UFUNCTION()
	void OnRep_Slots();

private:
	bool IsSlotIndexValid(int32 Index) const;
	bool IsSlotEmpty(int32 Index) const;
	int32 FindSlotIndex(FGuid ItemId) const;
	int32 FindFirstEmptyPosition() const;
	int32 ResolveTargetPosition(FItemPlacementRequest Request) const;

	UPROPERTY(ReplicatedUsing = OnRep_Slots)
	TArray<FItemData> Slots;
};
