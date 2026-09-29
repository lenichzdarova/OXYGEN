// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItemContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "ItemHoldingContainerRegistry.generated.h"

/**
 * 
 */
UCLASS()
class OXYGEN_API UItemHoldingContainerRegistry : public UWorldSubsystem
{
	GENERATED_BODY()	

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UObject* FindHolder(FGuid ItemId) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Claim(FGuid ItemId, UObject* NewHolder);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Release(FGuid ItemId, UObject* Holder);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SwapItems(FGuid ItemA, FGuid ItemB);
	

private:
	UPROPERTY()
	TMap<FGuid, UObject*> Locations;	
};
