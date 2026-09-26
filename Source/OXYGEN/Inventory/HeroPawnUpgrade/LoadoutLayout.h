#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "LoadoutLayout.generated.h"

USTRUCT(BlueprintType)
struct FLoadoutSlotDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	FGameplayTag SlotId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	FGameplayTagContainer AcceptedSets;
};

UCLASS(BlueprintType)
class OXYGEN_API ULoadoutLayout : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	TArray<FLoadoutSlotDef> Slots;
};