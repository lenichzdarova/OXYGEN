// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OxygenSaveGame.h"
#include "SaveManager.generated.h"

/**
 * Server-authoritative save manager.
 * One fixed slot per game session.
 * Periodic autosave + final save on session end.
 * Players cannot manually save/load.
 */
UCLASS()
class OXYGEN_API USaveManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Start a session for the given slot (load existing or create new). Call from GameMode on server. */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void StartSession(const FString& InSlotName);

	/** End the session: perform final save and stop timers. */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void EndSession();

	/** Trigger an immediate autosave (async). */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void PerformAutosave();

	UFUNCTION(BlueprintCallable)
	TArray<USaveGame*> GetSavedWorlds();

	UFUNCTION(BlueprintCallable)
	void SetSelectedWorld(USaveGame* World) { SelectedWorld = World; }

	const FString& GetCurrentSlot() const { return CurrentSlotName; }

protected:
	void LoadWorldFromSlot(const FString& SlotName);
	void CollectAndSave(bool bIsFinal = false);
	void OnAutosaveComplete(const FString& SlotName, const int32 UserIndex, bool bSuccess);

	/** Override / expand these to gather actual world and player state. */
	void CollectWorldData(UOxygenSaveGame* SaveObject);
	void CollectPlayerData(UOxygenSaveGame* SaveObject);
	void ApplyLoadedData(UOxygenSaveGame* SaveObject);

private:
	UPROPERTY()
	TObjectPtr<USaveGame> SelectedWorld;

	FString CurrentSlotName;
	FTimerHandle AutosaveTimerHandle;

	/** Autosave interval in seconds. Start low for load testing, raise in production. */
	UPROPERTY(EditDefaultsOnly, Category = "Save")
	float AutosaveIntervalSeconds = 60.0f;

	void GetSaveSlots(TArray<FString>& SaveSlots);
	void StartAutosaveTimer();
	void StopAutosaveTimer();
};
