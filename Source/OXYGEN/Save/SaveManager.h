// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveManager.generated.h"

/**
 * 
 */
UCLASS()
class OXYGEN_API USaveManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	
public:	
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION()
	void LoadWorld();
	UFUNCTION()
	void SaveWorld();	
	
	void LoadPlayer(const FUniqueNetId& PlayerId);
	void SavePlayer(const FUniqueNetId& PlayerId);
	
	UFUNCTION(BlueprintCallable)
	TArray<USaveGame*> GetSavedWorlds();
	
	UFUNCTION(BlueprintCallable)
	void SetSelectedWorld(USaveGame* World) { SelectedWorld = World; };
	
private:		
	
	UPROPERTY()
	USaveGame* SelectedWorld;
	
	void GetSaveSlots(TArray<FString>& SaveSlots);
};
