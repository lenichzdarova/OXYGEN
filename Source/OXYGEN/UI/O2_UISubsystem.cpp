// Fill out your copyright notice in the Description page of Project Settings.


#include "O2_UISubsystem.h"

#include "O2_UISettings.h"

void UO2_UISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Если Super уже создал Policy из DefaultUIPolicyClass — можно сбросить и заменить:
	// или не задавать DefaultUIPolicyClass в ini вообще

	if (GetCurrentUIPolicy())
	{
		return; // уже есть
	}

	const UO2_UISettings* Settings = GetDefault<UO2_UISettings>();
	if (!Settings || Settings->UIPolicyClass.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("O2_UISettings: UIPolicyClass not set"));
		return;
	}

	TSubclassOf<UGameUIPolicy> PolicyClass = Settings->UIPolicyClass.LoadSynchronous();
	if (!PolicyClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load UIPolicyClass"));
		return;
	}

	SwitchToPolicy(NewObject<UGameUIPolicy>(this, PolicyClass));
}
