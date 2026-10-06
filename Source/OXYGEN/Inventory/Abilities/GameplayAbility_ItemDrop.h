// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LyraInventoryManagerComponent.h"
#include "NativeGameplayTags.h"
#include "Abilities/LyraGameplayAbility.h"
#include "GameplayAbility_ItemDrop.generated.h"

USTRUCT(BlueprintType)
struct FOxygenDropItemTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid ItemId;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FOxygenDropItemTargetData::StaticStruct();
	}

	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << ItemId;
		bOutSuccess = true;
		return true;
	}
};

template<>
struct TStructOpsTypeTraits<FOxygenDropItemTargetData> : public TStructOpsTypeTraitsBase2<FOxygenDropItemTargetData>
{
	enum { WithNetSerializer = true };
};

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Inventory_Drop)
/**
 * 
 */
UCLASS()
class OXYGEN_API UGameplayAbility_ItemDrop : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
	UGameplayAbility_ItemDrop();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
private:	
	ULyraInventoryManagerComponent* GetInventoryManager();
	void CalculateSpawnTransfrom(FTransform& OutTransform);
	
};
