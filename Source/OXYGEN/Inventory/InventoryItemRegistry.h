// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LyraInventoryItemDefinition.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InventoryItemRegistry.generated.h"

/**
 * 
 */
UCLASS()
class OXYGEN_API UInventoryItemRegistry : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable)
	FGuid RegisterNewItem(TSubclassOf<ULyraInventoryItemDefinition> NewItemClass);
	
	UFUNCTION(BlueprintCallable)
	void UnregisterItem(FGuid ItemId);
	
	UFUNCTION(BlueprintCallable)
	ULyraInventoryItemInstance* GetItem(FGuid ItemId) {return *ItemRegistry.Find(ItemId);}
	
private:
	
	UPROPERTY()
	TMap<FGuid, ULyraInventoryItemInstance*> ItemRegistry;	
};
