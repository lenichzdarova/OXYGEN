// Fill out your copyright notice in the Description page of Project Settings.


#include "O2_PlayerController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

void AO2_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState.Get(), false);
	ASC->InitAbilityActorInfo(PlayerState.Get(), InPawn);
}
