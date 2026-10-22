// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingBlockStaticMeshComponent.h"
#include "GhostConnectorBase.h"
#include "HabitatConnectorComponent.h"
#include "BuildingPreviewActor.generated.h"

UCLASS(Abstract)
class OXYGEN_API ABuildingPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	ABuildingPreviewActor();
};
