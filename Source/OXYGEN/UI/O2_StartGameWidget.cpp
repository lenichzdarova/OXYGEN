// Fill out your copyright notice in the Description page of Project Settings.

#include "O2_StartGameWidget.h"
#include "O2_SessionSubsystem.h"
#include "O2_SaveManager.h"
#include "Kismet/GameplayStatics.h"

void UO2_StartGameWidget::NativeConstruct()
{	
	CreateButtonsForSavedWorlds();
	Super::NativeConstruct();
}

void UO2_StartGameWidget::CreateButtonsForSavedWorlds()
{	
	auto SaveManager = GetGameInstance()->GetSubsystem<UO2_SaveManager>();
	auto Saves = SaveManager->GetSavedGames();

	for (UO2_SaveGame* Save : Saves)
	{
		auto Widget = CreateWidget<UCommonButtonBase>(this, CreateSavedGameButtonClass);
		ButtonContainer->AddChildToVerticalBox(Widget);			
			Widget->OnClicked().AddLambda([this, Save]()
			{
				this->OnCreateSavedGameButtonClicked(Save);
			});
	}	
	auto Widget = CreateWidget<UCommonButtonBase>(this, CreateNewGameButton);
	ButtonContainer->AddChildToVerticalBox(Widget);
	Widget->OnClicked().AddUFunction(this, FName("OnCreateNewGameButtonClicked"));
}

void UO2_StartGameWidget::GetSaveGameNames(TArray<FString>& SlotNames)
{	
	// Путь к папке сохранений
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
}

void UO2_StartGameWidget::OnCreateNewGameButtonClicked()
{
	auto SessionSubsystem = GetGameInstance()->GetSubsystem<UO2_SessionSubsystem>();
	SessionSubsystem->OnCreateSessionCompleteEvent.AddUObject(this, &UO2_StartGameWidget::OnSessionCreateComplete);
	SessionSubsystem->HostNewGameSession(GetOwningPlayer());	
}

void UO2_StartGameWidget::OnSessionCreateComplete(const FOnlineResultInformation& Result)
{
}

void UO2_StartGameWidget::OnCreateSavedGameButtonClicked(UO2_SaveGame* InSavedGame)
{
	auto SessionSubsystem = GetGameInstance()->GetSubsystem<UO2_SessionSubsystem>();
	SessionSubsystem->HostNewGameSession(GetOwningPlayer());
}

