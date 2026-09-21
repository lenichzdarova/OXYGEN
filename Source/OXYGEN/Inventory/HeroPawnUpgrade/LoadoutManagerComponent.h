// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LyraAbilitySet.h"
#include "LyraInventoryItemInstance.h"
#include "Components/ActorComponent.h"
#include "Components/PawnComponent.h"
#include "Equipment/LyraEquipmentInstance.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "LoadoutManagerComponent.generated.h"

class ULoadoutManagerComponent;

USTRUCT(BlueprintType)
struct FLoadoutEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag SlotTag;

	UPROPERTY()
	TObjectPtr<ULyraInventoryItemInstance> ItemInstance = nullptr;

	UPROPERTY()
	TObjectPtr<ULyraEquipmentInstance> EquipmentInstance = nullptr;
};

USTRUCT(BlueprintType)
struct FLoadoutEntryList : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FLoadoutEntry> Entries;
	
	void AddEntry(ULyraInventoryItemInstance* Instance);
	void RemoveEntry(ULyraInventoryItemInstance* Instance);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FLoadoutEntry, FLoadoutEntryList>(
			Entries, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FLoadoutEntryList> : public TStructOpsTypeTraitsBase2<FLoadoutEntryList>
{
	enum { WithNetDeltaSerializer = true };
};

USTRUCT(BlueprintType)
struct FLoadoutChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Loadout")
	TObjectPtr<ULoadoutManagerComponent> Component = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Loadout")
	TArray<FLoadoutEntry> Entries;
};

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class ULoadoutManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	ULoadoutManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Поставить/заменить предмет по индексу. Index == -1 → добавить в конец. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Loadout")
	void Server_Equip(FGameplayTag SlotTag, ULyraInventoryItemInstance* ItemInstance);

	/** Снять по индексу. Возвращает снятый ItemInstance. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Loadout")
	void Unequip(FGameplayTag SlotTag);

	/** Поменять два entry местами (для DnD). */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Loadout")
	bool Swap(FGameplayTag SlotTagA, FGameplayTag SlotTagB);

	// --- Queries (UI) ---

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Loadout")
	int32 Num() const { return EntryList.Entries.Num(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Loadout")
	ULyraInventoryItemInstance* GetItemByTag(FGameplayTag SlotTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Loadout")
	ULyraEquipmentInstance* GetEquipmentByTag(FGameplayTag SlotTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Loadout")
	TArray<FLoadoutEntry> GetAllEntries() const { return EntryList.Entries; }	

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	
private:
	void RemoveItemFromPlayerInventory(ULyraInventoryItemInstance* InItemInstance);

	UPROPERTY(Replicated)
	FLoadoutEntryList EntryList;

	void NotifyLoadoutChanged();

	bool InternalAddEntry(ULyraInventoryItemInstance* ItemInstance, int32 AtIndex /* -1 = end */);
	void InternalRemoveEntry(int32 Index);

	ULyraAbilitySystemComponent* GetOwnerASC() const;
};
