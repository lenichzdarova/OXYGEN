// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "O2_PlayerController.generated.h"

class UCommonSession_SearchResult;
class UWorld;

/**
 * Player controller with Steam session Host / Find / Join helpers.
 */
UCLASS()
class OXYGEN_API AO2_PlayerController : public ACommonPlayerController
{
	GENERATED_BODY()

public:

protected:
	virtual void OnPossess(APawn* InPawn) override;
};
