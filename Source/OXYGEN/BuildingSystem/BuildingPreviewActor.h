// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingBlockStaticMeshComponent.h"
#include "GhostConnectorBase.h"
#include "HabitatConnectorComponent.h"
#include "BuildingPreviewActor.generated.h"



USTRUCT(BlueprintType)
struct FConnectorLengthOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Length = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AGhostConnectorBase> ConnectorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Tolerance = 15.f;
};

UCLASS(Abstract)
class OXYGEN_API ABuildingPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	ABuildingPreviewActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Building|Connectors")
	TArray<FConnectorLengthOption> ConnectorOptions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Building|Connectors")
	float MaxTraceDistance = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Building|Connectors")
	float AxisTolerance = 10.f;

	UFUNCTION(BlueprintCallable, Category="Building")
	void UpdateConnectorPreview();

	UFUNCTION(BlueprintCallable, Category="Building")
	void CommitConnectors();

	UFUNCTION(BlueprintCallable, Category="Building")
	void ClearPreviewConnectors();

protected:
	UPROPERTY()
	TArray<TObjectPtr<AGhostConnectorBase>> PreviewConnectors;

	TArray<UBuildingBlockStaticMeshComponent*> GetConnectorComponents() const;

	bool TryFindConnectorTarget( UBuildingBlockStaticMeshComponent* FromComp, UBuildingBlockStaticMeshComponent*& OutTarget, float& OutDistance) const;

	const FConnectorLengthOption* FindBestLengthOption(float Distance) const;

	void SpawnPreviewConnector(	UBuildingBlockStaticMeshComponent* From, UBuildingBlockStaticMeshComponent* To,	const FConnectorLengthOption& Option);
};
