// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OXYGEN/SaveSystem/O2_SaveManager.h"
#include "GameFramework/GameModeBase.h"
#include "O2_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class OXYGEN_API AO2_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AO2_GameMode();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

protected:
/*
	void LoadWorldState();
	void SaveWorldState();
	void LoadPlayerData(APlayerController* Player);
	void SavePlayerData(APlayerController* Player);*/

private:

	UPROPERTY()
	TObjectPtr<UO2_SaveManager> SaveManager;
};

