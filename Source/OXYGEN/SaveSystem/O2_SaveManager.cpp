// Fill out your copyright notice in the Description page of Project Settings.


#include "O2_SaveManager.h"

#include "Kismet/GameplayStatics.h"

void UO2_SaveManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

TArray<UO2_SaveGame*> UO2_SaveManager::GetSavedGames()
{
	TArray<UO2_SaveGame*> Result;	
	TArray<FString> SlotNames;
	
	const FString SaveGamesPath = FPaths::ProjectSavedDir() / TEXT("SaveGames");
	// Ищем все .sav файлы
	TArray<FString> FoundFiles;
	IFileManager& FileManager = IFileManager::Get();
	FileManager.FindFiles(FoundFiles, *(SaveGamesPath / TEXT("*.sav")), true, false);

	for (const FString& FileName : FoundFiles)
	{
		// Убираем расширение .sav → получаем имя слота
		const FString SlotName = FPaths::GetBaseFilename(FileName);
		SlotNames.Add(SlotName);
	}
	
	for (FString SaveSlotName : SlotNames)
	{
		if (auto SaveFile = UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0))
		{			
			UO2_SaveGame* InnerSave = Cast<UO2_SaveGame>(SaveFile);
			Result.Add(InnerSave);
		}		
	}
	
	return Result;
}
