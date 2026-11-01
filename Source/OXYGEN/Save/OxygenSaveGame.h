// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "OxygenSaveGame.generated.h"

/**
 * Data for a single building/habitat piece that needs to be restored.
 * Expand with construction progress, health, ownership, etc. as needed.
 */
USTRUCT(BlueprintType)
struct FSavedBuildingPiece
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FString ClassPath;

	UPROPERTY(SaveGame)
	FTransform Transform;

	UPROPERTY(SaveGame)
	float ConstructionProgress = 1.0f;

	UPROPERTY(SaveGame)
	FString UniqueId; // Guid as string or other identifier
};

/**
 * Minimal item data for inventory persistence.
 * Expand with stats, fragments, placement, etc.
 */
USTRUCT(BlueprintType)
struct FSavedItemData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FString ItemDefClassPath;

	UPROPERTY(SaveGame)
	FGuid ItemId;

	UPROPERTY(SaveGame)
	TMap<FName, int32> StatTags; // simplified
};

/**
 * Per-player persistent data (inventory, position, progress).
 */
USTRUCT(BlueprintType)
struct FSavedPlayerData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FString PlayerNetId;

	UPROPERTY(SaveGame)
	FTransform Transform;

	UPROPERTY(SaveGame)
	TArray<FSavedItemData> Inventory;

	UPROPERTY(SaveGame)
	float PlayTime = 0.f;
};

/**
 * Root save object for one game session / slot.
 * One slot per game instance. No free-form player saves.
 */
UCLASS()
class OXYGEN_API UOxygenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	int32 SaveVersion = 1;

	UPROPERTY(SaveGame)
	FString SlotName;

	UPROPERTY(SaveGame)
	FDateTime SaveTimestamp;

	UPROPERTY(SaveGame)
	float TotalPlayTime = 0.f;

	UPROPERTY(SaveGame)
	TArray<FSavedBuildingPiece> BuildingPieces;

	UPROPERTY(SaveGame)
	TArray<FSavedPlayerData> Players;

	// Add world-level state here (oxygen levels, objectives, etc.)
};
