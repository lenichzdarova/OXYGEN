// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "InventoryItemRegistry.h"
#include "LyraInventoryManagerComponent.h"
#include "Components/PawnComponent.h"
#include "Inventory/LyraInventoryItemInstance.h"

#include "LyraQuickBarComponent.generated.h"

class AActor;
class ULyraEquipmentInstance;
class ULyraEquipmentManagerComponent;
class UObject;
struct FFrame;

USTRUCT(BlueprintType)
struct FLyraQuickBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<ULyraQuickBarComponent> Component = nullptr;
};

USTRUCT(BlueprintType)
struct FLyraQuickBarActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<ULyraQuickBarComponent> Component = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 ActiveIndex = INDEX_NONE;
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class OXYGEN_API ULyraQuickBarComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	ULyraQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickBar")
	void CycleActiveSlotForward();

	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickBar")
	void CycleActiveSlotBackward();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory|QuickBar")
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory|QuickBar")
	bool AddItemToSlot(int32 SlotIndex, FGuid ItemId);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory|QuickBar")
	void Server_AddItemToSlot(int32 SlotIndex, FGuid ItemId);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory|QuickBar")
	FGuid RemoveItemFromSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickBar")
	const TArray<FItemData>& GetSlots() const { return Slots; }

	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickBar")
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickBar")
	FItemData GetActiveSlotItem() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickBar")
	int32 GetNextFreeItemSlot() const;

protected:
	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

private:
	void UnequipItemInSlot();
	void EquipItemInSlot();
	ULyraEquipmentManagerComponent* FindEquipmentManager() const;
	UInventoryItemRegistry* GetRegistry() const;
	bool HasAuthority() const;
	bool IsSlotEmpty(int32 SlotIndex) const;
	void NotifySlotsChanged();
	void NotifyActiveIndexChanged();
	UFUNCTION()
	void OnItemRemovedFromInventory(FGameplayTag Tag, const FLyraInventoryChangeMessage& Message);

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|QuickBar")
	int32 NumSlots = 5;

	UPROPERTY(ReplicatedUsing = OnRep_Slots)
	TArray<FItemData> Slots;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = INDEX_NONE;

	UPROPERTY()
	TObjectPtr<ULyraEquipmentInstance> EquippedItem;
};