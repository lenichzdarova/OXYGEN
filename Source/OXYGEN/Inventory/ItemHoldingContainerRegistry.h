// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItemContainer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
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
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TScriptInterface<IInventoryItemContainer> FindContainer(FGuid ItemId) const;

private:
	void OnItemTaken(FGameplayTag Channel, const FItemTakenMessage& Message);

	UPROPERTY()
	TMap<FGuid, TScriptInterface<IInventoryItemContainer>> Locations;

	FGameplayMessageListenerHandle ListenerHandle;
};
