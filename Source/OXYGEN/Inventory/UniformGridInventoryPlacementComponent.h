// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryPlacementManagerComponent.h"
#include "UniformGridInventoryPlacementComponent.generated.h"


UCLASS(BlueprintType, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class OXYGEN_API UUniformGridInventoryPlacementComponent : public UInventoryPlacementManagerComponent
{
	GENERATED_BODY()

public:
	UUniformGridInventoryPlacementComponent();
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Storage")
	int32 MaxSlots = 48;
	
	
	virtual bool CanPlaceItem(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest Request) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;	
		
	UFUNCTION(BlueprintCallable)
	TArray<ULyraInventoryItemInstance*> GetSlots() {return Slots;}
	
protected:
	virtual void Server_AddItem_Implementation(ULyraInventoryItemInstance* ItemInstance, FItemPlacementRequest Request) override;	
	virtual void Server_RemoveItem_Implementation(ULyraInventoryItemInstance* ItemInstance) override;	
	virtual void Server_MoveItemToSlot_Implementation(ULyraInventoryItemInstance* ItemInstance, FItemPlacementRequest Request) override;
	
	
private:
	
	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<TObjectPtr<ULyraInventoryItemInstance>> Slots;	
	
	UFUNCTION()
	void OnRep_Slots();
	
	int FindFirstEmptyPosition();
	void NotifySlotsChanged();
};
