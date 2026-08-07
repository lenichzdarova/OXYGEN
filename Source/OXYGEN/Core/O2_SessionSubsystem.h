// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonSessionSubsystem.h"
#include "O2_SessionSubsystem.generated.h"

class APlayerController;


/**
 * OXYGEN session facade over CommonSession + OnlineSubsystemSteam.
 * Provides Host / Find / Join with game defaults and console commands.
 */
UCLASS(Config = Game)
class OXYGEN_API UO2_SessionSubsystem : public UCommonSessionSubsystem
{
	GENERATED_BODY()

	public:
		UO2_SessionSubsystem();
	
	UFUNCTION(BlueprintCallable, Category = "OXYGEN|Session")
	void HostNewGameSession(APlayerController* HostingPlayer);
		
	virtual UCommonSession_HostSessionRequest* CreateOnlineHostSessionRequest() override;
	virtual UCommonSession_SearchSessionRequest* CreateOnlineSearchSessionRequest() override;
	
	
	

	UPROPERTY(Config, EditAnywhere, Category = "OXYGEN|Session")
	FName DefaultMapPrimaryName = TEXT("/Game/Maps/L_Test_Mechanics");

	UPROPERTY(Config, EditAnywhere, Category = "OXYGEN|Session")
	int32 DefaultMaxPlayers = 4;

	UPROPERTY(Config, EditAnywhere, Category = "OXYGEN|Session")
	FString DefaultModeName = TEXT("OXYGEN");
};
