// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/PawnComponent.h"
#include "Inventory/InventoryTypes.h"

#include "LyraQuickBarComponent.generated.h"

class ULyraEquipmentInstance;
class ULyraEquipmentManagerComponent;
class ULyraInventoryItemInstance;
class UInventoryItemRegistry;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class ULyraQuickBarComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	ULyraQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category="Lyra")
	void CycleActiveSlotForward();

	UFUNCTION(BlueprintCallable, Category="Lyra")
	void CycleActiveSlotBackward();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Lyra")
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="Lyra")
	TArray<FItemView> GetSlots() const { return Slots; }

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="Lyra")
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="Lyra")
	FItemView GetActiveSlotView() const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="Lyra")
	FGuid GetActiveSlotItemId() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Lyra")
	ULyraInventoryItemInstance* GetActiveSlotItem() const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="Lyra")
	int32 GetNextFreeItemSlot() const;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Lyra")
	void Server_AddItemToSlot(int32 SlotIndex, FGuid ItemId);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Lyra")
	void Server_RemoveItemFromSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Lyra")
	bool AddItemToSlot(int32 SlotIndex, FGuid ItemId);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Lyra")
	FItemView RemoveItemFromSlot(int32 SlotIndex);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Lyra")
	int32 NumSlots = 5;

	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

private:
	void UnequipItemInSlot();
	void EquipItemInSlot();
	ULyraEquipmentManagerComponent* FindEquipmentManager() const;
	UInventoryItemRegistry* GetRegistry() const;
	int32 FindItemIndex(FGuid ItemId) const;

	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<FItemView> Slots;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = -1;

	UPROPERTY()
	TObjectPtr<ULyraEquipmentInstance> EquippedItem;
};

USTRUCT(BlueprintType)
struct FLyraQuickBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<ULyraQuickBarComponent> Component = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TArray<FItemView> Slots;
};

USTRUCT(BlueprintType)
struct FLyraQuickBarActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<ULyraQuickBarComponent> Component = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 ActiveIndex = 0;
};
