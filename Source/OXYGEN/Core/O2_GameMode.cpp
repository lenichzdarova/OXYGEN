// Fill out your copyright notice in the Description page of Project Settings.


#include "O2_GameMode.h"

#include "O2_GameSession.h"
#include "O2_GameState.h"
#include "O2_PlayerController.h"
#include "O2_PlayerStateBase.h"

AO2_GameMode::AO2_GameMode()
{
	GameSessionClass = AO2_GameSession::StaticClass();
	GameStateClass = AO2_GameState::StaticClass();
	PlayerControllerClass = AO2_PlayerController::StaticClass();
	PlayerStateClass = AO2_PlayerStateBase::StaticClass();
	
	// DefaultPawnClass left to BP_GameMode / project settings so content can override.
}

void AO2_GameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AO2_GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AO2_GameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}
