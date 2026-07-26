// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "O2_SaveManager.generated.h"


class UWorldSaveGame;
class UPlayerSaveGame;
/**
 * 
 */
UCLASS()
class OXYGEN_API UO2_SaveManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()	

	public:

		virtual void Initialize(FSubsystemCollectionBase& Collection) override;

		// Создать новое сохранение
		void CreateSaveSlot(const FString& SlotName);

		// Загрузить сохранение
		bool LoadSaveSlot(const FString& SlotName);

		// Сохранить текущее состояние
		void SaveCurrentSlot();

		// Удалить сохранение
		void DeleteSaveSlot(const FString& SlotName);

		const FString& GetCurrentSlotName() const
		{
			return CurrentSlotName;
		}

private:

		// Текущий выбранный слот
		UPROPERTY()
		FString CurrentSlotName;

		UPROPERTY()
		TObjectPtr<UWorldSaveGame> CurrentWorldSave;

		UPROPERTY()
		TMap<FString, TObjectPtr<UPlayerSaveGame>> LoadedPlayerSaves;
	
};
