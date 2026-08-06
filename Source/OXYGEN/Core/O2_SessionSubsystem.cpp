// Fill out your copyright notice in the Description page of Project Settings.

#include "O2_SessionSubsystem.h"

UO2_SessionSubsystem::UO2_SessionSubsystem()
{
	bUseBeacons = false;
	bUseLobbiesDefault = true;
	bUseLobbiesVoiceChatDefault = false;
}

UCommonSession_HostSessionRequest* UO2_SessionSubsystem::CreateOnlineHostSessionRequest()
{
	UCommonSession_HostSessionRequest* Request = Super::CreateOnlineHostSessionRequest();
	Request->ModeNameForAdvertisement = DefaultModeName;
	Request->MaxPlayerCount = DefaultMaxPlayers;
	Request->MapID = FPrimaryAssetId(FPrimaryAssetType(TEXT("Map")), DefaultMapPrimaryName);
	Request->bUseLobbies = bUseLobbiesDefault;
	Request->bUsePresence = true;
	return Request;
}
