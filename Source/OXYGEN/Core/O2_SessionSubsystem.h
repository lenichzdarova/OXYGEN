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

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual UCommonSession_HostSessionRequest* CreateOnlineHostSessionRequest() override;

	/** Host a Steam lobby/session and travel to the default (or provided) map. */
	UFUNCTION(BlueprintCallable, Category = "OXYGEN|Session")
	void HostSessionEasy(APlayerController* HostingPlayer, FPrimaryAssetId MapId = FPrimaryAssetId(), int32 MaxPlayers = -1);

	/** Host using a soft map path (e.g. /Game/Maps/L_Test_Mechanics). */
	UFUNCTION(BlueprintCallable, Category = "OXYGEN|Session")
	void HostSessionOnMap(APlayerController* HostingPlayer, TSoftObjectPtr<UWorld> Map, int32 MaxPlayers = -1);

	/** Search Steam for joinable OXYGEN sessions/lobbies. */
	UFUNCTION(BlueprintCallable, Category = "OXYGEN|Session")
	void FindSessionsEasy(APlayerController* SearchingPlayer);

	/** Join a specific search result. */
	UFUNCTION(BlueprintCallable, Category = "OXYGEN|Session")
	void JoinSessionEasy(APlayerController* JoiningPlayer, UCommonSession_SearchResult* SearchResult);

	/** Join by index from the last FindSessionsEasy results. */
	UFUNCTION(BlueprintCallable, Category = "OXYGEN|Session")
	void JoinSessionByIndex(APlayerController* JoiningPlayer, int32 ResultIndex);

	UFUNCTION(BlueprintPure, Category = "OXYGEN|Session")
	const TArray<UCommonSession_SearchResult*>& GetLastSearchResults() const { return LastSearchResults; }

	UFUNCTION(BlueprintPure, Category = "OXYGEN|Session")
	FPrimaryAssetId GetDefaultMapId() const;

	UPROPERTY(BlueprintAssignable, Category = "OXYGEN|Session")
	FO2_OnFindSessionsComplete OnFindSessionsComplete;

	UPROPERTY(BlueprintAssignable, Category = "OXYGEN|Session")
	FO2_OnHostSessionComplete OnHostSessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "OXYGEN|Session")
	FO2_OnJoinSessionComplete OnJoinSessionComplete;

	/** Default map PrimaryAssetId name (Asset Manager type "Map"). */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "OXYGEN|Session")
	FName DefaultMapPrimaryName = TEXT("L_Test_Mechanics");

	/** Soft path fallback if Primary Asset lookup fails. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "OXYGEN|Session")
	TSoftObjectPtr<UWorld> DefaultMap = TSoftObjectPtr<UWorld>(FSoftObjectPath(TEXT("/Game/Maps/L_Test_Mechanics.L_Test_Mechanics")));

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "OXYGEN|Session")
	int32 DefaultMaxPlayers = 4;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "OXYGEN|Session")
	FString DefaultModeName = TEXT("OXYGEN");

protected:
	void HandleCreateSessionComplete(const FOnlineResultInformation& Result);
	void HandleJoinSessionComplete(const FOnlineResultInformation& Result);
	void HandleSearchFinished(bool bSucceeded, const FText& ErrorMessage);

	UCommonSession_HostSessionRequest* BuildHostRequest(FPrimaryAssetId MapId, int32 MaxPlayers) const;
	static FPrimaryAssetId MapIdFromSoftWorld(const TSoftObjectPtr<UWorld>& Map);

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCommonSession_SearchResult>> LastSearchResults;

	UPROPERTY(Transient)
	TObjectPtr<UCommonSession_SearchSessionRequest> ActiveSearchRequest;

	FDelegateHandle CreateSessionHandle;
	FDelegateHandle JoinSessionHandle;
};
