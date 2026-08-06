// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonSessionSubsystem.h"
#include "O2_SessionSubsystem.generated.h"

class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FO2_OnFindSessionsComplete, bool, bSucceeded, const TArray<UCommonSession_SearchResult*>&, Results);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FO2_OnHostSessionComplete, bool, bSucceeded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FO2_OnJoinSessionComplete, bool, bSucceeded);

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

	virtual UCommonSession_HostSessionRequest* CreateOnlineHostSessionRequest() override;

	UPROPERTY(Config, EditAnywhere, Category = "OXYGEN|Session")
	FName DefaultMapPrimaryName = TEXT("L_Test_Mechanics");

	UPROPERTY(Config, EditAnywhere, Category = "OXYGEN|Session")
	int32 DefaultMaxPlayers = 4;

	UPROPERTY(Config, EditAnywhere, Category = "OXYGEN|Session")
	FString DefaultModeName = TEXT("OXYGEN");
};
