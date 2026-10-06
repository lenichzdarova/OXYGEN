// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WorldSpawnedItem.generated.h"

class ULyraInventoryItemDefinition;
// This class does not need to be modified.
UINTERFACE()
class UWorldSpawnedItem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OXYGEN_API IWorldSpawnedItem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeFromItem(FGuid ItemId, TSubclassOf<ULyraInventoryItemDefinition> ItemDef);
};
