// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "O2_PlayerController.generated.h"

class UCommonSession_SearchResult;
class UWorld;

/**
 * Player controller with Steam session Host / Find / Join helpers.
 */
UCLASS()
class OXYGEN_API AO2_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** Host a Steam lobby/session (travels to map on success). */
	UFUNCTION(BlueprintCallable, Category = "OXYGEN|Session")
	void HostSteamSession(TSoftObjectPtr<UWorld> Map = nullptr, int32 MaxPlayers = -1);

	/** Search for Steam sessions. Results go to UO2_SessionSubsystem. */
	UFUNCTION(BlueprintCallable, Category = "OXYGEN|Session")
	void FindSteamSessions();

	/** Join a found session result. */
	UFUNCTION(BlueprintCallable, Category = "OXYGEN|Session")
	void JoinSteamSession(UCommonSession_SearchResult* SearchResult);

	/** Join by index from the last find results. */
	UFUNCTION(BlueprintCallable, Category = "OXYGEN|Session")
	void JoinSteamSessionByIndex(int32 ResultIndex = 0);

	/** Leave/destroy active session (e.g. return to menu). */
	UFUNCTION(BlueprintCallable, Category = "OXYGEN|Session")
	void LeaveSteamSession();

protected:
	virtual void OnPossess(APawn* InPawn) override;
};
