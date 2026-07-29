// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameSession.h"
#include "O2_GameSession.generated.h"

/**
 * Game-side session actor (max players / options).
 * Steam lobby creation lives in UO2_SessionSubsystem / CommonSession.
 */
UCLASS()
class OXYGEN_API AO2_GameSession : public AGameSession
{
	GENERATED_BODY()

public:
	AO2_GameSession();

	virtual void InitOptions(const FString& Options) override;
};
