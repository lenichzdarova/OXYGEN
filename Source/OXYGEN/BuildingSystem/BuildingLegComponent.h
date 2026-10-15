// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BuildingLegComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class OXYGEN_API UBuildingLegComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UBuildingLegComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Category="Leg")
	UStaticMesh* TopMeshAsset;
	
	UPROPERTY(EditAnywhere, Category="Leg")
	TObjectPtr<UMaterialInterface> TopMeshMaterial;

	UPROPERTY(EditAnywhere, Category="Leg")
	UStaticMesh* MidMeshAsset;

	UPROPERTY(EditAnywhere, Category="Leg")
	TObjectPtr<UMaterialInterface> MidMeshMaterial;
	
	UPROPERTY(EditAnywhere, Category="Leg")
	UStaticMesh* BottomMeshAsset;

	UPROPERTY(EditAnywhere, Category="Leg")
	TObjectPtr<UMaterialInterface> BottomMeshMaterial;
	
	UPROPERTY(EditAnywhere, Category="Leg")
	float DefaultMidLength = 100.f;

	UPROPERTY(EditAnywhere, Category="Leg")
	float MaxLength = 1000.f;

	UFUNCTION(BlueprintCallable)
	void UpdateLeg();

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TopMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MidMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BottomMesh;
	
	UPROPERTY(ReplicatedUsing=OnRep_LegScale)
	float LegScale = 1.f;

	virtual void OnRegister() override;
	
private:
	UFUNCTION()
	void OnRep_LegScale();
	
};
