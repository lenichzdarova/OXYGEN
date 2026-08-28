// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraPawn.h"

#include "GameFramework/Controller.h"
#include "LyraLogChannels.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ScriptInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraPawn)

class FLifetimeProperty;
class UObject;

ALyraPawn::ALyraPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ALyraPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}

void ALyraPawn::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ALyraPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ALyraPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);	
}

void ALyraPawn::UnPossessed()
{
	AController* const OldController = GetController();
	Super::UnPossessed();
}


