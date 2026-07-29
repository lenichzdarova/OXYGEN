// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonGameInstance.h"
#include "O2_GameInstance.generated.h"

/**
 * OXYGEN game instance.
 * Wires CommonUser login + CommonSession (Steam) invite/host/join flow.
 */
UCLASS(Config = Game)
class OXYGEN_API UO2_GameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

public:
	UO2_GameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Init() override;
	
	virtual TSubclassOf<UOnlineSession> GetOnlineSessionClass() override;

	/** Ensures primary local player is initialized for online play (Steam identity). */
	UFUNCTION(BlueprintCallable, Category = "OXYGEN|Online")
	bool EnsureOnlineUserReady(int32 LocalPlayerIndex = 0);
	
	
};
