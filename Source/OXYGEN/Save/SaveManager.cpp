// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveManager.h"

#include "OxygenSaveGame.h"
#include "Kismet/GameplayStatics.h"

void USaveManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
#if WITH_EDITOR
	auto Saves = GetSavedWorlds();
	if (Saves.IsEmpty())
	{
		for (int i = 0; i< 3 ; i++)
		{
			UOxygenSaveGame* Save = NewObject<UOxygenSaveGame>();
			UGameplayStatics::SaveGameToSlot(Save, FString::FromInt(i), 0);
		}	
	}
#endif
	
}

void USaveManager::LoadWorld()
{
}

void USaveManager::SaveWorld()
{
}

void USaveManager::LoadPlayer(const FUniqueNetId& PlayerId)
{
}

void USaveManager::SavePlayer(const FUniqueNetId& PlayerId)
{
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
	IFileManager::Get().FindFiles(Files,	*SaveDir,TEXT("sav"));
	
	for (const FString& File : Files)
	{
		SaveSlots.Add(FPaths::GetBaseFilename(File));
	}
}
