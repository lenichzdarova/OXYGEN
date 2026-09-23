#pragma once

#include "InventoryPlacementManagerComponent.h"

#include "UniformGridInventoryPlacementComponent.generated.h"

UCLASS(BlueprintType, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class OXYGEN_API UUniformGridInventoryPlacementComponent : public UInventoryPlacementManagerComponent
{
	GENERATED_BODY()

public:
	UUniformGridInventoryPlacementComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Storage")
	int32 MaxSlots = 48;

	virtual bool CanPlaceItem(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest Request) const override;
	virtual bool AddItem(FGuid ItemId, FItemPlacementRequest Request) override;
	virtual bool RemoveItem(FGuid ItemId) override;
	virtual bool MoveItemToSlot(FGuid ItemId, FItemPlacementRequest Request) override;
	virtual TArray<FItemView> GetSlots() const override { return Slots; }
	virtual bool ContainsItem(FGuid ItemId) const override;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	FItemView GetSlotView(int32 Index) const;

private:
	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<FItemView> Slots;

	UFUNCTION()
	void OnRep_Slots();

	int32 FindFirstEmptyPosition() const;
	int32 FindItemIndex(FGuid ItemId) const;
	void NotifySlotsChanged();
};
