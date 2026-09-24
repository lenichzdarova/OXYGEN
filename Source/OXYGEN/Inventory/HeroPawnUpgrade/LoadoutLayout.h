#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "LoadoutLayout.generated.h"

UENUM(BlueprintType)
enum class ELoadoutSlotPolicy : uint8
{
	ExactType,
	UniqueType,
	AllowDuplicate
};

USTRUCT(BlueprintType)
struct FLoadoutSlotDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	FGameplayTag SlotId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	FGameplayTagContainer AcceptedSets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout", meta = (EditCondition = "Policy == ELoadoutSlotPolicy::ExactType"))
	FGameplayTag ExactType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	ELoadoutSlotPolicy Policy = ELoadoutSlotPolicy::ExactType;
};

UCLASS(BlueprintType)
class OXYGEN_API ULoadoutLayout : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	TArray<FLoadoutSlotDef> Slots;
};