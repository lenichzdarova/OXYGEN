// Fill out your copyright notice in the Description page of Project Settings.

#include "O2_SessionSubsystem.h"

#include "O2_GameInstance.h"
#include "Online.h"
#include "OnlineSubsystemUtils.h"

UO2_SessionSubsystem::UO2_SessionSubsystem()
{
	bUseBeacons = false;
	bUseLobbiesDefault = true;
	bUseLobbiesVoiceChatDefault = false;
}

void UO2_SessionSubsystem::HostNewGameSession(APlayerController* HostingPlayer)
{	
	if (!HostingPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("HostNewGameSession: HostingPlayer is null"));
		return;
	}

	// Убеждаемся, что локальный игрок готов к онлайн
	if (UO2_GameInstance* O2GI = Cast<UO2_GameInstance>(GetGameInstance()))
	{
		O2GI->EnsureOnlineUserReady(0);
	}	
	auto Request = CreateOnlineHostSessionRequest();
	HostSession(HostingPlayer, Request);
}

UCommonSession_HostSessionRequest* UO2_SessionSubsystem::CreateOnlineHostSessionRequest()
{
	UCommonSession_HostSessionRequest* Request = Super::CreateOnlineHostSessionRequest();
	Request->MapID = FPrimaryAssetId(FPrimaryAssetType(TEXT("Map")), DefaultMapPrimaryName);	
	return Request;
}

UCommonSession_SearchSessionRequest* UO2_SessionSubsystem::CreateOnlineSearchSessionRequest()
{
	auto Request = Super::CreateOnlineSearchSessionRequest();	
	return Request;
}
