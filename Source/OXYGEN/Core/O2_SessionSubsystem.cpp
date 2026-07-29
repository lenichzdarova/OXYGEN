// Fill out your copyright notice in the Description page of Project Settings.

#include "O2_SessionSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Misc/PackageName.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(O2_SessionSubsystem)

DEFINE_LOG_CATEGORY_STATIC(LogO2Session, Log, All);

namespace O2SessionConsole
{
	static UO2_SessionSubsystem* Get(const UWorld* World)
	{
		if (!World)
		{
			return nullptr;
		}

		if (const UGameInstance* GI = World->GetGameInstance())
		{
			return GI->GetSubsystem<UO2_SessionSubsystem>();
		}

		return nullptr;
	}

	static APlayerController* GetPC(UWorld* World)
	{
		if (!World)
		{
			return nullptr;
		}

		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			return PC;
		}

		return nullptr;
	}

	static FAutoConsoleCommandWithWorld HostCmd(
		TEXT("O2.HostSession"),
		TEXT("Host a Steam session and travel to the default multiplayer map."),
		FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
		{
			if (UO2_SessionSubsystem* Session = Get(World))
			{
				Session->HostSessionEasy(GetPC(World));
			}
			else
			{
				UE_LOG(LogO2Session, Error, TEXT("O2.HostSession: session subsystem missing (is GameInstance set to O2_GameInstance?)"));
			}
		}));

	static FAutoConsoleCommandWithWorld FindCmd(
		TEXT("O2.FindSessions"),
		TEXT("Find Steam sessions/lobbies advertised by OXYGEN."),
		FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
		{
			if (UO2_SessionSubsystem* Session = Get(World))
			{
				Session->FindSessionsEasy(GetPC(World));
			}
			else
			{
				UE_LOG(LogO2Session, Error, TEXT("O2.FindSessions: session subsystem missing"));
			}
		}));

	static FAutoConsoleCommandWithWorldAndArgs JoinCmd(
		TEXT("O2.JoinSession"),
		TEXT("Join a session from the last find results. Usage: O2.JoinSession [index=0]"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			const int32 Index = Args.Num() > 0 ? FCString::Atoi(*Args[0]) : 0;
			if (UO2_SessionSubsystem* Session = Get(World))
			{
				Session->JoinSessionByIndex(GetPC(World), Index);
			}
			else
			{
				UE_LOG(LogO2Session, Error, TEXT("O2.JoinSession: session subsystem missing"));
			}
		}));

	static FAutoConsoleCommandWithWorld CleanupCmd(
		TEXT("O2.CleanupSessions"),
		TEXT("Destroy/leave any active Steam session."),
		FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
		{
			if (UO2_SessionSubsystem* Session = Get(World))
			{
				Session->CleanUpSessions();
			}
		}));
}

UO2_SessionSubsystem::UO2_SessionSubsystem()
{
	// Steam P2P lobby flow does not need party beacons for basic host/find/join.
	bUseBeacons = false;
	bUseLobbiesDefault = true;
	bUseLobbiesVoiceChatDefault = false;
}

void UO2_SessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CreateSessionHandle = OnCreateSessionCompleteEvent.AddUObject(this, &UO2_SessionSubsystem::HandleCreateSessionComplete);
	JoinSessionHandle = OnJoinSessionCompleteEvent.AddUObject(this, &UO2_SessionSubsystem::HandleJoinSessionComplete);

	UE_LOG(LogO2Session, Log, TEXT("O2 session subsystem ready (Steam lobbies=%s, beacons=%s)"),
		bUseLobbiesDefault ? TEXT("on") : TEXT("off"),
		bUseBeacons ? TEXT("on") : TEXT("off"));
}

void UO2_SessionSubsystem::Deinitialize()
{
	OnCreateSessionCompleteEvent.Remove(CreateSessionHandle);
	OnJoinSessionCompleteEvent.Remove(JoinSessionHandle);

	if (ActiveSearchRequest)
	{
		ActiveSearchRequest->OnSearchFinished.RemoveAll(this);
		ActiveSearchRequest = nullptr;
	}

	LastSearchResults.Reset();
	Super::Deinitialize();
}

UCommonSession_HostSessionRequest* UO2_SessionSubsystem::CreateOnlineHostSessionRequest()
{
	UCommonSession_HostSessionRequest* Request = Super::CreateOnlineHostSessionRequest();
	Request->ModeNameForAdvertisement = DefaultModeName;
	Request->MaxPlayerCount = DefaultMaxPlayers;
	Request->MapID = GetDefaultMapId();
	Request->bUseLobbies = bUseLobbiesDefault;
	Request->bUsePresence = true;
	return Request;
}

FPrimaryAssetId UO2_SessionSubsystem::GetDefaultMapId() const
{
	if (!DefaultMapPrimaryName.IsNone())
	{
		return FPrimaryAssetId(FPrimaryAssetType(TEXT("Map")), DefaultMapPrimaryName);
	}

	return MapIdFromSoftWorld(DefaultMap);
}

FPrimaryAssetId UO2_SessionSubsystem::MapIdFromSoftWorld(const TSoftObjectPtr<UWorld>& Map)
{
	if (Map.IsNull())
	{
		return FPrimaryAssetId();
	}

	const FSoftObjectPath Path = Map.ToSoftObjectPath();
	FString AssetName = Path.GetAssetName();
	if (AssetName.IsEmpty())
	{
		AssetName = FPackageName::GetShortName(Path.GetLongPackageName());
	}

	return FPrimaryAssetId(FPrimaryAssetType(TEXT("Map")), FName(*AssetName));
}

UCommonSession_HostSessionRequest* UO2_SessionSubsystem::BuildHostRequest(FPrimaryAssetId MapId, int32 MaxPlayers) const
{
	UCommonSession_HostSessionRequest* Request = const_cast<UO2_SessionSubsystem*>(this)->CreateOnlineHostSessionRequest();

	if (MapId.IsValid())
	{
		Request->MapID = MapId;
	}

	if (MaxPlayers > 0)
	{
		Request->MaxPlayerCount = MaxPlayers;
	}

	return Request;
}

void UO2_SessionSubsystem::HostSessionEasy(APlayerController* HostingPlayer, FPrimaryAssetId MapId, int32 MaxPlayers)
{
	if (!HostingPlayer)
	{
		HostingPlayer = GetGameInstance() ? GetGameInstance()->GetFirstLocalPlayerController() : nullptr;
	}

	if (!HostingPlayer)
	{
		UE_LOG(LogO2Session, Error, TEXT("HostSessionEasy: no local player controller"));
		OnHostSessionComplete.Broadcast(false);
		return;
	}

	UCommonSession_HostSessionRequest* Request = BuildHostRequest(MapId.IsValid() ? MapId : GetDefaultMapId(), MaxPlayers);
	UE_LOG(LogO2Session, Log, TEXT("HostSessionEasy: map=%s max=%d"), *Request->MapID.ToString(), Request->MaxPlayerCount);
	HostSession(HostingPlayer, Request);
}

void UO2_SessionSubsystem::HostSessionOnMap(APlayerController* HostingPlayer, TSoftObjectPtr<UWorld> Map, int32 MaxPlayers)
{
	HostSessionEasy(HostingPlayer, MapIdFromSoftWorld(Map), MaxPlayers);
}

void UO2_SessionSubsystem::FindSessionsEasy(APlayerController* SearchingPlayer)
{
	if (!SearchingPlayer)
	{
		SearchingPlayer = GetGameInstance() ? GetGameInstance()->GetFirstLocalPlayerController() : nullptr;
	}

	if (!SearchingPlayer)
	{
		UE_LOG(LogO2Session, Error, TEXT("FindSessionsEasy: no local player controller"));
		OnFindSessionsComplete.Broadcast(false, TArray<UCommonSession_SearchResult*>());
		return;
	}

	if (ActiveSearchRequest)
	{
		ActiveSearchRequest->OnSearchFinished.RemoveAll(this);
		ActiveSearchRequest = nullptr;
	}

	LastSearchResults.Reset();

	ActiveSearchRequest = CreateOnlineSearchSessionRequest();
	ActiveSearchRequest->OnSearchFinished.AddUObject(this, &UO2_SessionSubsystem::HandleSearchFinished);

	UE_LOG(LogO2Session, Log, TEXT("FindSessionsEasy: searching Steam sessions/lobbies..."));
	FindSessions(SearchingPlayer, ActiveSearchRequest);
}

void UO2_SessionSubsystem::JoinSessionEasy(APlayerController* JoiningPlayer, UCommonSession_SearchResult* SearchResult)
{
	if (!JoiningPlayer)
	{
		JoiningPlayer = GetGameInstance() ? GetGameInstance()->GetFirstLocalPlayerController() : nullptr;
	}

	if (!JoiningPlayer || !SearchResult)
	{
		UE_LOG(LogO2Session, Error, TEXT("JoinSessionEasy: invalid player or search result"));
		OnJoinSessionComplete.Broadcast(false);
		return;
	}

	UE_LOG(LogO2Session, Log, TEXT("JoinSessionEasy: joining %s"), *SearchResult->GetDescription());
	JoinSession(JoiningPlayer, SearchResult);
}

void UO2_SessionSubsystem::JoinSessionByIndex(APlayerController* JoiningPlayer, int32 ResultIndex)
{
	if (!LastSearchResults.IsValidIndex(ResultIndex))
	{
		UE_LOG(LogO2Session, Error, TEXT("JoinSessionByIndex: index %d out of range (found %d)"), ResultIndex, LastSearchResults.Num());
		OnJoinSessionComplete.Broadcast(false);
		return;
	}

	JoinSessionEasy(JoiningPlayer, LastSearchResults[ResultIndex]);
}

void UO2_SessionSubsystem::HandleCreateSessionComplete(const FOnlineResultInformation& Result)
{
	if (Result.bWasSuccessful)
	{
		UE_LOG(LogO2Session, Log, TEXT("Host session created successfully"));
	}
	else
	{
		UE_LOG(LogO2Session, Error, TEXT("Host session failed: %s (%s)"), *Result.ErrorText.ToString(), *Result.ErrorId);
	}

	OnHostSessionComplete.Broadcast(Result.bWasSuccessful);
}

void UO2_SessionSubsystem::HandleJoinSessionComplete(const FOnlineResultInformation& Result)
{
	if (Result.bWasSuccessful)
	{
		UE_LOG(LogO2Session, Log, TEXT("Join session succeeded (traveling...)"));
	}
	else
	{
		UE_LOG(LogO2Session, Error, TEXT("Join session failed: %s (%s)"), *Result.ErrorText.ToString(), *Result.ErrorId);
	}

	OnJoinSessionComplete.Broadcast(Result.bWasSuccessful);
}

void UO2_SessionSubsystem::HandleSearchFinished(bool bSucceeded, const FText& ErrorMessage)
{
	LastSearchResults.Reset();

	if (ActiveSearchRequest)
	{
		LastSearchResults = ActiveSearchRequest->Results;
		ActiveSearchRequest->OnSearchFinished.RemoveAll(this);
		ActiveSearchRequest = nullptr;
	}

	if (bSucceeded)
	{
		UE_LOG(LogO2Session, Log, TEXT("FindSessions finished: %d result(s)"), LastSearchResults.Num());
		for (int32 i = 0; i < LastSearchResults.Num(); ++i)
		{
			if (UCommonSession_SearchResult* Result = LastSearchResults[i])
			{
				UE_LOG(LogO2Session, Log, TEXT("  [%d] %s  open=%d/%d  ping=%dms"),
					i,
					*Result->GetDescription(),
					Result->GetNumOpenPublicConnections(),
					Result->GetMaxPublicConnections(),
					Result->GetPingInMs());
			}
		}
	}
	else
	{
		UE_LOG(LogO2Session, Error, TEXT("FindSessions failed: %s"), *ErrorMessage.ToString());
	}

	TArray<UCommonSession_SearchResult*> ResultsCopy;
	ResultsCopy.Reserve(LastSearchResults.Num());
	for (UCommonSession_SearchResult* Result : LastSearchResults)
	{
		ResultsCopy.Add(Result);
	}

	OnFindSessionsComplete.Broadcast(bSucceeded, ResultsCopy);
}
