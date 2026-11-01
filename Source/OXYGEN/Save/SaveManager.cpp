// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveManager.h"

#include "OxygenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "LyraLogChannels.h"

void USaveManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if WITH_EDITOR
	auto Saves = GetSavedWorlds();
	if (Saves.IsEmpty())
	{
		for (int32 i = 0; i < 3; ++i)
		{
			UOxygenSaveGame* Save = NewObject<UOxygenSaveGame>();
			Save->SlotName = FString::FromInt(i);
			UGameplayStatics::SaveGameToSlot(Save, Save->SlotName, 0);
		}
	}
#endif
}

void USaveManager::Deinitialize()
{
	StopAutosaveTimer();
	Super::Deinitialize();
}

void USaveManager::StartSession(const FString& InSlotName)
{
	if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
	{
		UE_LOG(LogLyra, Warning, TEXT("SaveManager::StartSession called on client — ignored."));
		return;
	}

	CurrentSlotName = InSlotName.IsEmpty() ? TEXT("DefaultSession") : InSlotName;

	LoadWorldFromSlot(CurrentSlotName);
	StartAutosaveTimer();

	UE_LOG(LogLyra, Log, TEXT("Save session started for slot: %s"), *CurrentSlotName);
}

void USaveManager::EndSession()
{
	if (CurrentSlotName.IsEmpty())
	{
		return;
	}

	StopAutosaveTimer();
	CollectAndSave(true);

	UE_LOG(LogLyra, Log, TEXT("Save session ended for slot: %s"), *CurrentSlotName);
	CurrentSlotName.Empty();
}

void USaveManager::PerformAutosave()
{
	CollectAndSave(false);
}

void USaveManager::LoadWorldFromSlot(const FString& SlotName)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		if (UOxygenSaveGame* Loaded = Cast<UOxygenSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0)))
		{
			ApplyLoadedData(Loaded);
			UE_LOG(LogLyra, Log, TEXT("Loaded existing save for slot %s (version %d)"), *SlotName, Loaded->SaveVersion);
			return;
		}
	}

	// New session
	UOxygenSaveGame* NewSave = NewObject<UOxygenSaveGame>();
	NewSave->SlotName = SlotName;
	NewSave->SaveTimestamp = FDateTime::UtcNow();
	ApplyLoadedData(NewSave); // no-op for new
	UE_LOG(LogLyra, Log, TEXT("Created new save for slot %s"), *SlotName);
}

void USaveManager::CollectAndSave(bool bIsFinal)
{
	if (CurrentSlotName.IsEmpty() || !GetWorld() || GetWorld()->GetNetMode() == NM_Client)
	{
		return;
	}

	UOxygenSaveGame* SaveObject = NewObject<UOxygenSaveGame>();
	SaveObject->SlotName = CurrentSlotName;
	SaveObject->SaveTimestamp = FDateTime::UtcNow();
	SaveObject->SaveVersion = 1;

	CollectWorldData(SaveObject);
	CollectPlayerData(SaveObject);

	// Async save to avoid hitches (best practice for autosave)
	FAsyncSaveGameToSlotDelegate Delegate;
	Delegate.BindUObject(this, &USaveManager::OnAutosaveComplete);
	UGameplayStatics::AsyncSaveGameToSlot(SaveObject, CurrentSlotName, 0, Delegate);

	UE_LOG(LogLyra, Verbose, TEXT("%s save triggered for slot %s"), bIsFinal ? TEXT("Final") : TEXT("Auto"), *CurrentSlotName);
}

void USaveManager::OnAutosaveComplete(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	if (bSuccess)
	{
		UE_LOG(LogLyra, Verbose, TEXT("Autosave succeeded for %s"), *SlotName);
	}
	else
	{
		UE_LOG(LogLyra, Error, TEXT("Autosave FAILED for %s"), *SlotName);
	}
}

void USaveManager::CollectWorldData(UOxygenSaveGame* SaveObject)
{
	// TODO: Iterate HabitatActorBase / Building pieces via BaseManagerSubsystem or GetAllActorsOfClass
	// Fill SaveObject->BuildingPieces
	// Example skeleton:
	// for (TActorIterator<AHabitatActorBase> It(GetWorld()); It; ++It)
	// {
	//     FSavedBuildingPiece Piece;
	//     Piece.ClassPath = It->GetClass()->GetPathName();
	//     Piece.Transform = It->GetActorTransform();
	//     // Piece.ConstructionProgress = ...
	//     SaveObject->BuildingPieces.Add(Piece);
	// }
}

void USaveManager::CollectPlayerData(UOxygenSaveGame* SaveObject)
{
	// TODO: For each PlayerController / PlayerState
	// Gather transform, inventory from ULyraInventoryManagerComponent, etc.
	// Example:
	// for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	// {
	//     if (APlayerController* PC = It->Get())
	//     {
	//         FSavedPlayerData PlayerData;
	//         if (APlayerState* PS = PC->PlayerState)
	//             PlayerData.PlayerNetId = PS->GetUniqueId().ToString();
	//         if (APawn* Pawn = PC->GetPawn())
	//             PlayerData.Transform = Pawn->GetActorTransform();
	//         // Inventory...
	//         SaveObject->Players.Add(PlayerData);
	//     }
	// }
}

void USaveManager::ApplyLoadedData(UOxygenSaveGame* SaveObject)
{
	// TODO: Spawn / restore buildings from SaveObject->BuildingPieces
	// Restore player data when they join (or here for already present)
	// Version check: if (SaveObject->SaveVersion < CurrentVersion) Migrate(...);
}

void USaveManager::StartAutosaveTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			AutosaveTimerHandle,
			this,
			&USaveManager::PerformAutosave,
			AutosaveIntervalSeconds,
			true);
	}
}

void USaveManager::StopAutosaveTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutosaveTimerHandle);
	}
}

TArray<USaveGame*> USaveManager::GetSavedWorlds()
{
	TArray<USaveGame*> Result;
	TArray<FString> Slots;
	GetSaveSlots(Slots);

	for (const FString& SlotName : Slots)
	{
		if (USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(SlotName, 0))
		{
			Result.Add(SaveGame);
		}
	}

	return Result;
}

void USaveManager::GetSaveSlots(TArray<FString>& SaveSlots)
{
	const FString SaveDir = FPaths::ProjectSavedDir() / TEXT("SaveGames");
	TArray<FString> Files;
	IFileManager::Get().FindFiles(Files, *SaveDir, TEXT("sav"));

	for (const FString& File : Files)
	{
		SaveSlots.Add(FPaths::GetBaseFilename(File));
	}
}
