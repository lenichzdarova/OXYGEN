// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingBlockStaticMeshComponent.h"
#include "HabitatConnectorComponent.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Building_Mesh_Type_Connector)

UCLASS(ClassGroup=(Building), meta=(BlueprintSpawnableComponent))
class OXYGEN_API UHabitatConnectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHabitatConnectorComponent();

	UFUNCTION(BlueprintCallable, Category="Building|Connector")
	void InitConnection(AActor* ModuleA, AActor* ModuleB);

	UFUNCTION(BlueprintCallable, Category="Building|Connector")
	void RestoreConnections();

	UFUNCTION(BlueprintCallable, Category="Building|Connector")
	bool IsConnected() const;
	
	UFUNCTION()
	UBuildingBlockStaticMeshComponent* FindConnectorComponent(AActor* Module) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Building|Connector")
	float TraceDistance = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Building|Connector")
	float AxisTolerance = 20.f;

	UPROPERTY(BlueprintReadOnly, Category="Building|Connector")
	TWeakObjectPtr<AActor> ModuleA;

	UPROPERTY(BlueprintReadOnly, Category="Building|Connector")
	TWeakObjectPtr<AActor> ModuleB;

	UPROPERTY()
	TWeakObjectPtr<UBuildingBlockStaticMeshComponent> CompA;

	UPROPERTY()
	TWeakObjectPtr<UBuildingBlockStaticMeshComponent> CompB;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool TraceForComponent(const FVector& Direction, UBuildingBlockStaticMeshComponent*& OutComp) const;
	
	UBuildingBlockStaticMeshComponent* FindClosestConnectorComponent(AActor* Module, const FVector& NearLocation) const;

	UFUNCTION()
	void OnModuleDestroyed(AActor* DestroyedActor);
};
