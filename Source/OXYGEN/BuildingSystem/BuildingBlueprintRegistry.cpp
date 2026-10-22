// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingBlueprintRegistry.h"
#include "OxygenBuildingBlueprintDefinition.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/AssetManager.h"

void UBuildingBlueprintRegistry::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CollectDefinitions();
}

void UBuildingBlueprintRegistry::CollectDefinitions()
{
	Definitions.Empty();

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> AssetDataList;
	AssetRegistry.GetAssetsByClass(UOxygenBuildingBlueprintDefinition::StaticClass()->GetClassPathName(), AssetDataList);

	for (const FAssetData& AssetData : AssetDataList)
	{
		UOxygenBuildingBlueprintDefinition* Definition = Cast<UOxygenBuildingBlueprintDefinition>(AssetData.GetAsset());
		if (!Definition)
		{
			continue;
		}

		if (!Definition->Id.IsValid())
		{
			continue;
		}

		Definitions.Add(Definition->Id, Definition);
	}
}

UOxygenBuildingBlueprintDefinition* UBuildingBlueprintRegistry::GetDefinition(FGameplayTag BlueprintTag) const
{
	if (const TObjectPtr<UOxygenBuildingBlueprintDefinition>* Found = Definitions.Find(BlueprintTag))
	{
		return Found->Get();
	}
	return nullptr;
}

TArray<UOxygenBuildingBlueprintDefinition*> UBuildingBlueprintRegistry::GetAllDefinitions() const
{
	TArray<UOxygenBuildingBlueprintDefinition*> Result;
	Result.Reserve(Definitions.Num());

	for (const TPair<FGameplayTag, TObjectPtr<UOxygenBuildingBlueprintDefinition>>& Pair : Definitions)
	{
		if (Pair.Value)
		{
			Result.Add(Pair.Value.Get());
		}
	}

	return Result;
}

bool UBuildingBlueprintRegistry::HasDefinition(FGameplayTag BlueprintTag) const
{
	return Definitions.Contains(BlueprintTag);
}