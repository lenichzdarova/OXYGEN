// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryItemContainer.generated.h"


// This class does not need to be modified.
UINTERFACE()
class UInventoryItemContainer : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OXYGEN_API IInventoryItemContainer
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	void Extract(FGuid ItemId);

	void NotifyItemTaken(FGuid ItemId);	
};

USTRUCT(BlueprintType)
struct FItemTakenMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FGuid ItemId;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TScriptInterface<IInventoryItemContainer> Container;
};
