#pragma once

#include "InventoryItemContainer.h"
#include "Components/PawnComponent.h"
#include "InventoryItemRegistry.h"
#include "LoadoutLayout.h"
#include "LoadoutManagerComponent.generated.h"

class ULyraEquipmentInstance;
class ULyraEquipmentManagerComponent;
class ULoadoutManagerComponent;
class UInventoryFragment_LoadoutItem;
class ULyraInventoryItemDefinition;

USTRUCT(BlueprintType)
struct FLoadoutChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Loadout")
	TObjectPtr<ULoadoutManagerComponent> Component = nullptr;
};

USTRUCT(BlueprintType)
struct FLoadoutSlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag SlotId;

	UPROPERTY(BlueprintReadOnly)
	FItemData Item;
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class OXYGEN_API ULoadoutManagerComponent : public UPawnComponent, public IInventoryItemContainer
{
	GENERATED_BODY()

public:
	ULoadoutManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool CanEquip(FGuid ItemId, FGameplayTag SlotId) const;

	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool CanEquipDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FGameplayTag SlotId) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Loadout")
	bool EquipItem(FGuid ItemId, FGameplayTag SlotId);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Loadout")
	void Server_EquipItem(FGuid ItemId, FGameplayTag SlotId);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Loadout")
	FGuid UnequipItem(FGameplayTag SlotId);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Loadout")
	void Server_UnequipItem(FGameplayTag SlotId);

	UFUNCTION(BlueprintCallable, Category = "Loadout")
	const TArray<FLoadoutSlot>& GetSlots() const { return Slots; }

	UFUNCTION(BlueprintCallable, Category = "Loadout")
	FLoadoutSlotDef GetSlotDef(FGameplayTag SlotId) const;

	UFUNCTION(BlueprintCallable, Category = "Loadout")
	ULyraEquipmentInstance* GetEquipment(FGameplayTag SlotId) const;

protected:
	UFUNCTION()
	void OnRep_Slots();

private:
	const UInventoryFragment_LoadoutItem* FindFragment(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const;
	bool MatchesSlot(const UInventoryFragment_LoadoutItem& Fragment, FGameplayTag SlotId) const;
	ULyraEquipmentManagerComponent* FindEquipmentManager() const;
	UInventoryItemRegistry* GetRegistry() const;
	//bool HasAuthority() const;
	void ApplyEquipment(FGameplayTag SlotId, FGuid ItemId, const UInventoryFragment_LoadoutItem& Fragment);
	void RemoveEquipment(FGameplayTag SlotId);
	int GetSlotIndexByID(FGameplayTag SlotId) const;
	void NotifyChanged();

	UPROPERTY(EditDefaultsOnly, Category = "Loadout")
	TObjectPtr<ULoadoutLayout> Layout;

	UPROPERTY(ReplicatedUsing = OnRep_Slots)
	TArray<FLoadoutSlot> Slots;

	UPROPERTY()
	TArray<TObjectPtr<ULyraEquipmentInstance>> Equipment;
	
	//IInventoryContainer interface
public:
	virtual void Extract_Implementation(FGuid ItemId) override;
};