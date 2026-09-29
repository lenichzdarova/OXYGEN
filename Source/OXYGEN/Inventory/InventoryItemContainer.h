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

class OXYGEN_API IInventoryItemContainer
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	void Extract(FGuid ItemId);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	bool CanInsertItem(FGuid Old, FGuid New);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	void InsertItem(FGuid Old, FGuid New);
	
};

