// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "System/GameplayTagStack.h"
#include "Templates/SubclassOf.h"

#include "LyraInventoryItemInstance.generated.h"

class FLifetimeProperty;

class ULyraInventoryItemDefinition;
class ULyraInventoryItemFragment;
class UInventoryItemRegistry;
struct FFrame;
struct FGameplayTag;

/**
 * Server-only runtime item. Not the networked source of truth.
 * Clients see FItemView instead.
 */
UCLASS(BlueprintType)
class ULyraInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	ULyraInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool IsSupportedForNetworking() const override { return false; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= Inventory)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool HasStatTag(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category=Inventory)
	FGuid GetItemId() const { return ItemId; }

	TSubclassOf<ULyraInventoryItemDefinition> GetItemDef() const
	{
		return ItemDef;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass))
	const ULyraInventoryItemFragment* FindFragmentByClass(TSubclassOf<ULyraInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

private:
	void SetItemDef(TSubclassOf<ULyraInventoryItemDefinition> InDef);
	void SetItemId(FGuid InId);

	friend class UInventoryItemRegistry;

	UPROPERTY()
	FGameplayTagStackContainer StatTags;

	UPROPERTY()
	TSubclassOf<ULyraInventoryItemDefinition> ItemDef;

	UPROPERTY()
	FGuid ItemId;
};
