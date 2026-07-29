// Fill out your copyright notice in the Description page of Project Settings.

#include "O2_PlayerController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CommonSessionSubsystem.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "O2_SessionSubsystem.h"

void AO2_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState.Get(), false))
	{
		ASC->InitAbilityActorInfo(PlayerState.Get(), InPawn);
	}
}

void AO2_PlayerController::HostSteamSession(TSoftObjectPtr<UWorld> Map, int32 MaxPlayers)
{
	if (UO2_SessionSubsystem* Session = GetGameInstance()->GetSubsystem<UO2_SessionSubsystem>())
	{
		if (Map.IsNull())
		{
			Session->HostSessionEasy(this, FPrimaryAssetId(), MaxPlayers);
		}
		else
		{
			Session->HostSessionOnMap(this, Map, MaxPlayers);
		}
	}
}

void AO2_PlayerController::FindSteamSessions()
{
	if (UO2_SessionSubsystem* Session = GetGameInstance()->GetSubsystem<UO2_SessionSubsystem>())
	{
		Session->FindSessionsEasy(this);
	}
}

void AO2_PlayerController::JoinSteamSession(UCommonSession_SearchResult* SearchResult)
{
	if (UO2_SessionSubsystem* Session = GetGameInstance()->GetSubsystem<UO2_SessionSubsystem>())
	{
		Session->JoinSessionEasy(this, SearchResult);
	}
}

void AO2_PlayerController::JoinSteamSessionByIndex(int32 ResultIndex)
{
	if (UO2_SessionSubsystem* Session = GetGameInstance()->GetSubsystem<UO2_SessionSubsystem>())
	{
		Session->JoinSessionByIndex(this, ResultIndex);
	}
}

void AO2_PlayerController::LeaveSteamSession()
{
	if (UO2_SessionSubsystem* Session = GetGameInstance()->GetSubsystem<UO2_SessionSubsystem>())
	{
		Session->CleanUpSessions();
	}
}
