// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "O2_SaveManager.generated.h"

/** Абстрактные данные одного игрока внутри общего сейва */
USTRUCT(BlueprintType)
struct FO2_PlayerSaveEntry
{
	GENERATED_BODY()

	/** UniqueNetId.ToString() или другой стабильный id */
	UPROPERTY(SaveGame, BlueprintReadOnly)
	FString UniquePlayerId;

	/** Произвольный payload игрока */
	UPROPERTY(SaveGame)
	TArray<uint8> PlayerData;
};

/** Один слот = мир + все игроки этой сессии */
UCLASS()
class OXYGEN_API UO2_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame, BlueprintReadOnly)
	FString SlotName;

	/** Payload мира */
	UPROPERTY(SaveGame)
	TArray<uint8> WorldData;	

	/** Все игроки, известные этому сейву */
	UPROPERTY(SaveGame)
	TMap<FString, FO2_PlayerSaveEntry> Players;
};


UCLASS()
class OXYGEN_API UO2_SaveManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()	

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void SaveGame();
	void LoadGame();
	
private:
	
	void CollectWorldData();
	void CollectActorData();

	
private:
	UPROPERTY()
	FString CurrentSlotName;		
};
