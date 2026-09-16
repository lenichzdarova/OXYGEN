// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryPlacementManagerComponent.generated.h"

class ULyraInventoryItemDefinition;
class ULyraInventoryItemInstance;
class UInventoryPlacementManagerComponent;


USTRUCT(BlueprintType)
struct FPlacementManagerChangeMessage
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UInventoryPlacementManagerComponent> SenderComponent = nullptr;	
	
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TArray<ULyraInventoryItemInstance*> Slots;
};


USTRUCT(BlueprintType)
struct FItemPlacementRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ULyraInventoryItemInstance> ItemInstance = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	int32 Position = -1; // -1 = auto

	UPROPERTY(BlueprintReadWrite)
	int32 Rotation = 0;

	// опционально
	UPROPERTY(BlueprintReadWrite)
	bool bAllowAutoAdjust = true; // можно ли сдвинуть/повернуть
};

UCLASS(Abstract, BlueprintType, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class OXYGEN_API UInventoryPlacementManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable)
	virtual bool CanPlaceItem(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest Request);
	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_AddItem(ULyraInventoryItemInstance* ItemInstance, FItemPlacementRequest Request);
	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_RemoveItem(ULyraInventoryItemInstance* ItemInstance);	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_MoveItemToSlot(ULyraInventoryItemInstance* ItemInstance, FItemPlacementRequest Request);
};


