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

};
