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
