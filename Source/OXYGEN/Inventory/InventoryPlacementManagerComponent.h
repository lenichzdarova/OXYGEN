#pragma once

#include "InventoryTypes.h"
#include "Components/ActorComponent.h"

#include "InventoryPlacementManagerComponent.generated.h"

class ULyraInventoryItemDefinition;
class UInventoryItemRegistry;

USTRUCT(BlueprintType)
struct FPlacementManagerChangeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UInventoryPlacementManagerComponent> SenderComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TArray<FItemView> Slots;
};

UCLASS(Abstract, BlueprintType, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class OXYGEN_API UInventoryPlacementManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryPlacementManagerComponent();

	UFUNCTION(BlueprintCallable, Category = Inventory)
	virtual bool CanPlaceItem(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, FItemPlacementRequest Request) const;

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void Server_AddItem(FGuid ItemId, FItemPlacementRequest Request);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void Server_RemoveItem(FGuid ItemId);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void Server_MoveItemToSlot(FGuid ItemId, FItemPlacementRequest Request);

	virtual bool AddItem(FGuid ItemId, FItemPlacementRequest Request);
	virtual bool RemoveItem(FGuid ItemId);
	virtual bool MoveItemToSlot(FGuid ItemId, FItemPlacementRequest Request);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	virtual TArray<FItemView> GetSlots() const { return TArray<FItemView>(); }

	UFUNCTION(BlueprintCallable, Category = Inventory)
	virtual bool ContainsItem(FGuid ItemId) const { return false; }

protected:
	virtual void Server_AddItem_Implementation(FGuid ItemId, FItemPlacementRequest Request);
	virtual void Server_RemoveItem_Implementation(FGuid ItemId);
	virtual void Server_MoveItemToSlot_Implementation(FGuid ItemId, FItemPlacementRequest Request);

	bool HasAuthority() const;
	UInventoryItemRegistry* GetRegistry() const;
};
