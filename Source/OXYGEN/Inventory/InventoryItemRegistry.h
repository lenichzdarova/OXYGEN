// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "System/GameplayTagStack.h"
#include "Templates/SubclassOf.h"
#include "InventoryItemRegistry.generated.h"

class ULyraInventoryItemDefinition;
class ULyraInventoryItemInstance;

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGuid ItemId;

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<ULyraInventoryItemDefinition> Def;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTagStackContainer StatTags;

	bool IsValid() const
	{
		return ItemId.IsValid() && Def != nullptr;
	}
};

UCLASS()
class OXYGEN_API UInventoryItemRegistry : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory|Registry")
	FGuid CreateItem(TSubclassOf<ULyraInventoryItemDefinition> ItemDef);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Registry")
	void DestroyItem(FGuid ItemId);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Registry")
	bool HasItem(FGuid ItemId) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Registry")
	ULyraInventoryItemInstance* GetItem(FGuid ItemId) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Registry")
	FItemData MakeData(FGuid ItemId) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Registry")
	TSubclassOf<ULyraInventoryItemDefinition> GetItemDef(FGuid ItemId) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Registry")
	bool IsAuthority() const;

private:
	UPROPERTY()
	TMap<FGuid, TObjectPtr<ULyraInventoryItemInstance>> Items;
};
