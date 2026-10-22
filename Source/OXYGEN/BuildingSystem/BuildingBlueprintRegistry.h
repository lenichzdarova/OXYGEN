// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BuildingBlueprintRegistry.generated.h"


class UOxygenBuildingBlueprintDefinition;
/**
 * 
 */
UCLASS()
class OXYGEN_API UBuildingBlueprintRegistry : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category="Building")
	UOxygenBuildingBlueprintDefinition* GetDefinition(FGameplayTag BlueprintTag) const;

	UFUNCTION(BlueprintCallable, Category="Building")
	TArray<UOxygenBuildingBlueprintDefinition*> GetAllDefinitions() const;

	UFUNCTION(BlueprintCallable, Category="Building")
	bool HasDefinition(FGameplayTag BlueprintTag) const;

private:
	void CollectDefinitions();

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UOxygenBuildingBlueprintDefinition>> Definitions;
};
