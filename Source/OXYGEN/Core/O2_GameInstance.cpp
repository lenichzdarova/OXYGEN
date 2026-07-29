// Fill out your copyright notice in the Description page of Project Settings.

#include "O2_GameInstance.h"

#include "CommonLocalPlayer.h"
#include "CommonUserSubsystem.h"
#include "O2_OnlineSession.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(O2_GameInstance)

UO2_GameInstance::UO2_GameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{	
		// CommonGame expects CommonLocalPlayer for UI/session plumbing.	
}

void UO2_GameInstance::Init()
{
	Super::Init();

	// Kick offline->online login for player 0 so Steam sessions have a valid net id.
	EnsureOnlineUserReady(0);
}

TSubclassOf<UOnlineSession> UO2_GameInstance::GetOnlineSessionClass()
{
	return UO2_OnlineSession::StaticClass();
}

bool UO2_GameInstance::EnsureOnlineUserReady(int32 LocalPlayerIndex)
{
	UCommonUserSubsystem* UserSubsystem = GetSubsystem<UCommonUserSubsystem>();
	if (!UserSubsystem)
	{
		return false;
	}

	const ECommonUserInitializationState State = UserSubsystem->GetLocalPlayerInitializationState(LocalPlayerIndex);
	if (State == ECommonUserInitializationState::LoggedInOnline
		|| State == ECommonUserInitializationState::DoingNetworkLogin
		|| State == ECommonUserInitializationState::DoingInitialLogin)
	{
		return true;
	}

	return UserSubsystem->TryToLoginForOnlinePlay(LocalPlayerIndex);
}
