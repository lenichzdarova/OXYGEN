// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModularPawn.h"
#include "LyraPawn.generated.h"

#define UE_API OXYGEN_API

class AController;
class UObject;
struct FFrame;

/**
 * ALyraPawn
 */
UCLASS(MinimalAPI)
class ALyraPawn : public AModularPawn
{
	GENERATED_BODY()

public:

	UE_API ALyraPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	UE_API virtual void PreInitializeComponents() override;
	UE_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~APawn interface
	UE_API virtual void PossessedBy(AController* NewController) override;
	UE_API virtual void UnPossessed() override;
	//~End of APawn interface
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

};

#undef UE_API
