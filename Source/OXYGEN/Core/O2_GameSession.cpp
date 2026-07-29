// Fill out your copyright notice in the Description page of Project Settings.

#include "O2_GameSession.h"

#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(O2_GameSession)

AO2_GameSession::AO2_GameSession()
{
	// Matches UO2_SessionSubsystem::DefaultMaxPlayers
	MaxPlayers = 4;
	MaxSpectators = 0;
}

void AO2_GameSession::InitOptions(const FString& Options)
{
	Super::InitOptions(Options);

	MaxPlayers = UGameplayStatics::GetIntOption(Options, TEXT("MaxPlayers"), MaxPlayers);
}
