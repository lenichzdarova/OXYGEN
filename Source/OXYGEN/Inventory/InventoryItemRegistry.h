#pragma once

#include "InventoryTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "InventoryItemRegistry.generated.h"

class ULyraInventoryItemDefinition;
class ULyraInventoryItemInstance;

/**
 * Server-authoritative item catalog and instance factory.
 * Item UObjects live here and are NOT replicated.
 * Clients read FItemView from inventory / placement / quickbar components.
 */
UCLASS()
class OXYGEN_API UInventoryItemRegistry : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	bool IsAuthority() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	FGuid CreateItem(TSubclassOf<ULyraInventoryItemDefinition> NewItemClass, int32 StackCount = 1, FGuid ExistingId = FGuid());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	FGuid AdoptInstance(ULyraInventoryItemInstance* Instance, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void DestroyItem(FGuid ItemId);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	ULyraInventoryItemInstance* GetItem(FGuid ItemId) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool GetView(FGuid ItemId, FItemView& OutView) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	FItemView MakeView(FGuid ItemId) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetStackCount(FGuid ItemId) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void SetStackCount(FGuid ItemId, int32 NewCount);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool Contains(FGuid ItemId) const;

private:
	FGuid RegisterInstance(ULyraInventoryItemInstance* Instance, int32 StackCount, FGuid ExistingId);

	UPROPERTY()
	TMap<FGuid, FInventoryItemRecord> ItemRegistry;
};
